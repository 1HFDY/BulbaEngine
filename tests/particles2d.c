#include "test_common.h"
#include "tests.h"

int BLB_TestParticles2D(void) {
  BLB_TestContext app;
  if (BLB_TestContext_Init(&app, 1200, 900, "BulbaEngine - Particles 2D", HMM_V3(0.0f, 0.0f, 10.0f), 7, 9, 15) != 0)
    return -1;

  BLB_ParticleSystem2D *system = BLB_CreateParticleSystem2D(
      HMM_V2(1.0f, 1.0f), HMM_V2(600.0f, 650.0f), 0, 2048, 2.6f, 0.8f, NULL, true);
  if (!system) {
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  system->gravity = HMM_V2(0.0f, 75.0f);
  system->velocity = HMM_V2(0.0f, -210.0f);
  system->velocity_variation = HMM_V2(120.0f, 80.0f);
  system->particle_scale = HMM_V2(7.0f, 7.0f);
  system->particle_scale_variation = HMM_V2(2.5f, 2.0f);
  system->color = HMM_V4(255.0f, 225.0f, 105.0f, 230.0f);
  system->color_end = HMM_V4(255.0f, 60.0f, 25.0f, 0.0f);
  system->emission_rate = 180.0f;
  system->rotation_speed = 90.0f;
  system->looping = true;


  if (!BLB_ParticleSystem2D_SetCount(system, 256)) {
    BLB_DestroyParticleSystem2D(system);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  if (BLB_AddParticleSystem2D(app.scene, system) != 0) {
    BLB_DestroyParticleSystem2D(system);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  BLB_Particle2D *single = BLB_CreateParticle2D(
      (BLB_ParticleID){.id = BLB_INVALID_PARTICLE_ID}, HMM_V2(430.0f, 620.0f), HMM_V2(115.0f, -95.0f), HMM_V2(15.0f, 15.0f),
      0.0f, -45.0f, HMM_V4(80.0f, 190.0f, 255.0f, 235.0f), 3.5f, 3.5f, NULL, NULL, true);

  if (single) {
    if (BLB_AddParticle2D(app.scene, single) != 0) {
      BLB_DestroyParticle2D(single);
      single = NULL;
    }
  }

  while (!BLB_WindowShouldClose(app.window)) {
    float dt = 0.0f;
    int frame = BLB_TestContext_BeginFrame(&app, &dt);
    if (frame < 0)
      break;
    if (frame > 0)
      continue;
    if (BLB_TestContext_Draw(&app) < 0)
      break;
  }

  if (single) {
    BLB_RemoveParticle2D(app.scene, single);
    BLB_DestroyParticle2D(single);
  }

  BLB_RemoveParticleSystem2D(app.scene, system);
  BLB_DestroyParticleSystem2D(system);
  BLB_TestContext_Shutdown(&app);
  return 0;
}
