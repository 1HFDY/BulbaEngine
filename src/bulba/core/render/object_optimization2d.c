#include "bulba/core/render/object_optimization2d.h"

#include "bulba/core/render/object_lod.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static BLB_RenderMode object_render_mode(const BLB_Object2D *object) {
  if (!object)
    return BLB_RENDER_OPAQUE;
  return object->material ? object->material->render_mode : object->render_mode;
}

static const BLB_Polygon2D *active_polygon(const BLB_Object2D *object) {
  return BLB_Object2D_GetLODPolygon(object);
}

static uint64_t signature(BLB_Object2D **objects, size_t count) {
  uint64_t h = UINT64_C(1469598103934665603);
  for (size_t i = 0; i < count; ++i) {
    BLB_Object2D *o = objects ? objects[i] : NULL;
    uintptr_t values[4] = {
        (uintptr_t)o,
        (uintptr_t)active_polygon(o),
        (uintptr_t)(o ? o->material : NULL),
        (uintptr_t)(o ? o->texture : NULL),
    };
    for (size_t j = 0; j < 4; ++j) {
      h ^= (uint64_t)values[j];
      h *= UINT64_C(1099511628211);
    }
    if (o) {
      h ^= (uint64_t)o->lod_level; h *= UINT64_C(1099511628211);
      h ^= (uint64_t)o->layer; h *= UINT64_C(1099511628211);
      h ^= (uint64_t)object_render_mode(o); h *= UINT64_C(1099511628211);
      h ^= o->screen_space ? 1u : 0u; h *= UINT64_C(1099511628211);
    }
  }
  return h ^ (uint64_t)count;
}

static bool can_instance(const BLB_Object2D *object) {
  const BLB_Polygon2D *polygon = active_polygon(object);
  if (!object || !polygon || !object->material)
    return false;
  if (object_render_mode(object) != BLB_RENDER_OPAQUE)
    return false;
  if (object->material->shader_program)
    return false;
  if (object->animation && object->animation->enable)
    return false;
  if (object->glow > 0.00001f || object->emission > 0.00001f || (object->material->emission_strength > 0.00001f) || (object->material->glow_strength > 0.00001f))
    return false;
  return polygon->vertex_count > 0 && polygon->indices && polygon->index_count >= 3;
}

static bool key_equal(const BLB_RenderInstances2DGroup *group, const BLB_Object2D *object) {
  if (!group || !object)
    return false;
  const BLB_Polygon2D *polygon = active_polygon(object);
  return group->polygon == polygon && group->material == object->material && group->texture == object->texture &&
         group->layer == object->layer && group->render_mode == object_render_mode(object) && group->screen_space == object->screen_space;
}

static uint64_t object_map_hash(const BLB_Object2D *object) {
  uintptr_t value = (uintptr_t)object;
  uint64_t hash = (uint64_t)value;
  hash ^= hash >> 30;
  hash *= UINT64_C(0xbf58476d1ce4e5b9);
  hash ^= hash >> 27;
  hash *= UINT64_C(0x94d049bb133111eb);
  hash ^= hash >> 31;
  return hash;
}

static size_t next_power_of_two(size_t value) {
  size_t capacity = 16;
  while (capacity < value) {
    if (capacity > SIZE_MAX / 2u)
      return 0;
    capacity <<= 1u;
  }
  return capacity;
}

static BLB_RenderInstances2DGroup *find_linear(BLB_RenderInstances2DGroup **groups, size_t count, const BLB_Object2D *object) {
  for (size_t i = 0; i < count; ++i)
    if (key_equal(groups[i], object))
      return groups[i];
  return NULL;
}

static BLB_RenderInstances2DGroup *group_map_find(const BLB_ObjectOptimization2D *optimization, const BLB_Object2D *object) {
  if (!optimization || !object || !optimization->group_map_capacity || !optimization->group_map_keys)
    return NULL;
  size_t mask = optimization->group_map_capacity - 1u;
  size_t index = (size_t)(object_map_hash(object) & mask);
  for (size_t probes = 0; probes < optimization->group_map_capacity; ++probes) {
    const BLB_Object2D *key = optimization->group_map_keys[index];
    if (!key)
      return NULL;
    if (key == object)
      return optimization->group_map_values[index];
    index = (index + 1u) & mask;
  }
  return NULL;
}

static bool build_group_map(BLB_ObjectOptimization2D *optimization) {
  if (!optimization)
    return false;
  size_t capacity = next_power_of_two(optimization->group_count * 2u + 1u);
  if (!capacity)
    return false;
  optimization->group_map_keys = calloc(capacity, sizeof(*optimization->group_map_keys));
  optimization->group_map_values = calloc(capacity, sizeof(*optimization->group_map_values));
  if (!optimization->group_map_keys || !optimization->group_map_values)
    return false;
  optimization->group_map_capacity = capacity;
  for (size_t i = 0; i < optimization->group_count; ++i) {
    BLB_RenderInstances2DGroup *group = optimization->groups[i];
    if (!group)
      continue;
    for (size_t j = 0; j < group->object_count; ++j) {
      BLB_Object2D *object = group->objects[j];
      size_t mask = capacity - 1u;
      size_t index = (size_t)(object_map_hash(object) & mask);
      while (optimization->group_map_keys[index])
        index = (index + 1u) & mask;
      optimization->group_map_keys[index] = object;
      optimization->group_map_values[index] = group;
    }
  }
  return true;
}

static bool add_object(BLB_RenderInstances2DGroup *group, BLB_Object2D *object) {
  if (group->object_count >= group->object_capacity) {
    size_t capacity = group->object_capacity ? group->object_capacity * 2u : 8u;
    BLB_Object2D **objects = realloc(group->objects, capacity * sizeof(*objects));
    if (!objects)
      return false;
    group->objects = objects;
    group->object_capacity = capacity;
  }
  group->objects[group->object_count++] = object;
  return true;
}

BLB_ObjectOptimization2D *BLB_ObjectOptimization2D_Create(BLB_Object2D **objects, size_t count) {
  BLB_ObjectOptimization2D *result = calloc(1, sizeof(*result));
  if (!result)
    return NULL;
  result->source_count = count;
  result->source_signature = signature(objects, count);
  if (count) {
    result->source_objects = malloc(count * sizeof(*result->source_objects));
    if (!result->source_objects) { free(result); return NULL; }
    memcpy(result->source_objects, objects, count * sizeof(*result->source_objects));
  }
  for (size_t i = 0; i < count; ++i) {
    BLB_Object2D *object = objects ? objects[i] : NULL;
    if (!can_instance(object))
      continue;
    BLB_RenderInstances2DGroup *group = find_linear(result->groups, result->group_count, object);
    if (!group) {
      group = calloc(1, sizeof(*group));
      if (!group)
        continue;
      group->polygon = active_polygon(object);
      group->material = object->material;
      group->texture = object->texture;
      group->shader = object->material ? object->material->shader_program : NULL;
      group->layer = object->layer;
      group->render_mode = object_render_mode(object);
      memcpy(group->color, object->color, sizeof(group->color));
      group->screen_space = object->screen_space;
      group->enabled = true;
      BLB_RenderInstances2DGroup **groups = realloc(result->groups, (result->group_count + 1u) * sizeof(*groups));
      if (!groups) { free(group); continue; }
      result->groups = groups;
      result->groups[result->group_count++] = group;
    }
    if (!add_object(group, object))
      group->enabled = false;
  }
  build_group_map(result);
  return result;
}

void BLB_ObjectOptimization2D_Destroy(BLB_ObjectOptimization2D *optimization) {
  if (!optimization)
    return;
  for (size_t i = 0; i < optimization->group_count; ++i) {
    free(optimization->groups[i]->objects);
    free(optimization->groups[i]);
  }
  free(optimization->group_map_keys);
  free(optimization->group_map_values);
  free(optimization->groups);
  free(optimization->source_objects);
  free(optimization);
}

bool BLB_ObjectOptimization2D_MatchesSource(const BLB_ObjectOptimization2D *optimization, BLB_Object2D **objects, size_t count) {
  if (!optimization || optimization->source_count != count)
    return false;
  return optimization->source_signature == signature(objects, count);
}

BLB_RenderInstances2DGroup *BLB_ObjectOptimization2D_FindGroup(const BLB_ObjectOptimization2D *optimization, const BLB_Object2D *object) {
  if (!optimization || !object)
    return NULL;
  BLB_RenderInstances2DGroup *group = group_map_find(optimization, object);
  return group ? group : find_linear(optimization->groups, optimization->group_count, object);
}

void BLB_ObjectOptimization2D_ResetDrawState(BLB_ObjectOptimization2D *optimization) {
  if (!optimization)
    return;
  for (size_t i = 0; i < optimization->group_count; ++i)
    optimization->groups[i]->rendered = false;
}
