#include "bulba/core/render/render_async.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

typedef struct BLB_RenderAsyncJob {
  BLB_RenderAsyncRun run;
  BLB_RenderAsyncComplete complete;
  BLB_RenderAsyncDestroy destroy;
  void *user_data;
  struct BLB_RenderAsyncJob *next;
} BLB_RenderAsyncJob;

#define BLB_RENDER_ASYNC_MAX_PENDING 64u

typedef struct {
  pthread_t *threads;
  size_t thread_count;
  BLB_RenderAsyncJob *queue_head;
  BLB_RenderAsyncJob *queue_tail;
  BLB_RenderAsyncJob *complete_head;
  BLB_RenderAsyncJob *complete_tail;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  bool initialized;
  bool stopping;
  size_t pending;
} BLB_RenderAsyncState;

static BLB_RenderAsyncState state;

static void *worker_main(void *unused) {
  (void)unused;

  for (;;) {
    pthread_mutex_lock(&state.mutex);

    while (!state.stopping && !state.queue_head)
      pthread_cond_wait(&state.cond, &state.mutex);

    if (state.stopping && !state.queue_head) {
      pthread_mutex_unlock(&state.mutex);
      break;
    }

    BLB_RenderAsyncJob *job = state.queue_head;
    state.queue_head = job->next;
    if (!state.queue_head)
      state.queue_tail = NULL;
    job->next = NULL;

    pthread_mutex_unlock(&state.mutex);

    if (job->run)
      job->run(job->user_data);

    pthread_mutex_lock(&state.mutex);
    if (state.pending > 0)
      --state.pending;

    if (job->complete) {
      if (state.complete_tail)
        state.complete_tail->next = job;
      else
        state.complete_head = job;
      state.complete_tail = job;
    } else {
      BLB_RenderAsyncDestroy destroy = job->destroy;
      void *user_data = job->user_data;
      pthread_mutex_unlock(&state.mutex);
      if (destroy)
        destroy(user_data);
      free(job);
      continue;
    }

    pthread_mutex_unlock(&state.mutex);
  }

  return NULL;
}

static size_t default_worker_count(void) {
  long count = sysconf(_SC_NPROCESSORS_ONLN);
  if (count <= 2)
    return 1;
  size_t workers = (size_t)count - 2u;
  if (workers > 4u)
    workers = 4u;
  return workers;
}

int BLB_RenderAsync_Init(size_t worker_count) {
  if (state.initialized)
    return 0;

  if (worker_count == 0)
    worker_count = default_worker_count();
  if (worker_count == 0)
    worker_count = 1;

  state.threads = calloc(worker_count, sizeof(*state.threads));
  if (!state.threads)
    return -1;

  if (pthread_mutex_init(&state.mutex, NULL) != 0) {
    free(state.threads);
    state.threads = NULL;
    return -1;
  }

  if (pthread_cond_init(&state.cond, NULL) != 0) {
    pthread_mutex_destroy(&state.mutex);
    free(state.threads);
    state.threads = NULL;
    return -1;
  }

  state.thread_count = worker_count;
  state.initialized = true;
  state.stopping = false;

  size_t started = 0;
  for (; started < worker_count; ++started) {
    if (pthread_create(&state.threads[started], NULL, worker_main, NULL) != 0)
      break;
  }

  if (started == 0) {
    state.stopping = true;
    pthread_cond_broadcast(&state.cond);
    pthread_cond_destroy(&state.cond);
    pthread_mutex_destroy(&state.mutex);
    free(state.threads);
    state.threads = NULL;
    state.thread_count = 0;
    state.initialized = false;
    return -1;
  }

  state.thread_count = started;
  return 0;
}

void BLB_RenderAsync_Shutdown(void) {
  if (!state.initialized)
    return;

  pthread_mutex_lock(&state.mutex);
  state.stopping = true;
  pthread_cond_broadcast(&state.cond);
  pthread_mutex_unlock(&state.mutex);

  for (size_t i = 0; i < state.thread_count; ++i)
    pthread_join(state.threads[i], NULL);

  BLB_RenderAsyncJob *job = state.queue_head;
  while (job) {
    BLB_RenderAsyncJob *next = job->next;
    if (job->destroy)
      job->destroy(job->user_data);
    free(job);
    job = next;
  }

  job = state.complete_head;
  while (job) {
    BLB_RenderAsyncJob *next = job->next;
    if (job->destroy)
      job->destroy(job->user_data);
    free(job);
    job = next;
  }

  state.queue_head = NULL;
  state.queue_tail = NULL;
  state.complete_head = NULL;
  state.complete_tail = NULL;
  state.pending = 0;
  state.stopping = false;
  state.initialized = false;

  pthread_cond_destroy(&state.cond);
  pthread_mutex_destroy(&state.mutex);
  free(state.threads);
  state.threads = NULL;
  state.thread_count = 0;
}

int BLB_RenderAsync_Submit(BLB_RenderAsyncRun run, BLB_RenderAsyncComplete complete, BLB_RenderAsyncDestroy destroy, void *user_data) {
  if (!run || !state.initialized)
    return -1;

  BLB_RenderAsyncJob *job = calloc(1, sizeof(*job));
  if (!job)
    return -1;

  job->run = run;
  job->complete = complete;
  job->destroy = destroy;
  job->user_data = user_data;

  pthread_mutex_lock(&state.mutex);
  if (state.stopping || state.pending >= BLB_RENDER_ASYNC_MAX_PENDING) {
    pthread_mutex_unlock(&state.mutex);
    free(job);
    return -1;
  }

  if (state.queue_tail)
    state.queue_tail->next = job;
  else
    state.queue_head = job;
  state.queue_tail = job;
  ++state.pending;
  pthread_cond_signal(&state.cond);
  pthread_mutex_unlock(&state.mutex);

  return 0;
}

size_t BLB_RenderAsync_Poll(void) {
  if (!state.initialized)
    return 0;

  size_t count = 0;
  for (;;) {
    pthread_mutex_lock(&state.mutex);
    BLB_RenderAsyncJob *job = state.complete_head;
    if (job) {
      state.complete_head = job->next;
      if (!state.complete_head)
        state.complete_tail = NULL;
      job->next = NULL;
    }
    pthread_mutex_unlock(&state.mutex);

    if (!job)
      break;

    if (job->complete)
      job->complete(job->user_data);
    if (job->destroy)
      job->destroy(job->user_data);
    free(job);
    ++count;
  }

  return count;
}

size_t BLB_RenderAsync_GetPending(void) {
  if (!state.initialized)
    return 0;

  pthread_mutex_lock(&state.mutex);
  size_t pending = state.pending;
  pthread_mutex_unlock(&state.mutex);
  return pending;
}

void BLB_RenderAsync_WaitAll(void) {
  if (!state.initialized)
    return;

  for (;;) {
    BLB_RenderAsync_Poll();
    if (BLB_RenderAsync_GetPending() == 0) {
      BLB_RenderAsync_Poll();
      if (BLB_RenderAsync_GetPending() == 0)
        break;
    }
    sched_yield();
  }
}
