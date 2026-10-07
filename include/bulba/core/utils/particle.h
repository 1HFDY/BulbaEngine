#ifndef PARTICLE_H
#define PARTICLE_H

#define BLB_INVALID_PARTICLE_ID UINT32_MAX
#define BLB_INVALID_PARTICLE_SYSTEM_ID UINT32_MAX

#include <stdint.h>

typedef struct {
  uint32_t id;
  char type[12];
} BLB_ParticleID;

typedef struct {
  uint32_t id;
  char type[12];
} BLB_ParticleSystemID;

#endif
