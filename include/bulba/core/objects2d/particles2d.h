#ifndef PARTICLES2D_H
#define PARTICLES2D_H

#include "bulba/core/entity.h"
#include "bulba/core/math3v/HandmadeMath.h"
#include "bulba/core/math3v/polygon.h"
#include "bulba/core/render/material.h"
#include "bulba/core/render/texture.h"
#include "bulba/core/utils/particle.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  BLB_ParticleID id;

  HMM_Vec2 position;
  HMM_Vec2 velocity;
  HMM_Vec2 scale;

  float rotation;
  float rotation_speed;

  HMM_Vec4 color;

  float lifetime;
  float lifetime_max;
  float delta_time;

  int render_mode;
  BLB_ComponentMask component_mask;
  bool visible;
  bool screen_space;

  BLB_Texture *texture;
  BLB_Material *material;

  BLB_Polygon2D *polygon;
} BLB_Particle2D;

typedef struct {
  BLB_Particle2D *particles;
  BLB_ParticleSystemID id;

  size_t count;
  size_t capacity;

  float emission_rate;
  float emission_accumulator;

  float lifetime;
  float lifetime_variation;
  float delta_time;

  int render_mode;
  BLB_ComponentMask component_mask;
  bool visible;
  bool screen_space;

  HMM_Vec2 position;
  HMM_Vec2 velocity;
  HMM_Vec2 velocity_variation;
  HMM_Vec2 gravity;

  HMM_Vec2 particle_system_scale;
  HMM_Vec2 particle_scale;
  HMM_Vec2 particle_scale_variation;

  HMM_Vec4 color;
  HMM_Vec4 color_end;

  float rotation;
  float rotation_speed;

  bool looping;
  bool playing;

  BLB_Texture *texture;
  BLB_Material *material;
} BLB_ParticleSystem2D;

BLB_Particle2D *BLB_CreateParticle2D(BLB_ParticleID id, HMM_Vec2 position, HMM_Vec2 velocity, HMM_Vec2 scale, float rotation, float rotation_speed,
                                     HMM_Vec4 color, float lifetime, float lifetime_max, BLB_Texture *texture, BLB_Material *material,
                                     bool screen_space);

void BLB_DestroyParticle2D(BLB_Particle2D *p);

void BLB_Particle2D_Move(BLB_Particle2D *p, HMM_Vec2 velocity);
void BLB_Particle2D_SetPosition(BLB_Particle2D *p, HMM_Vec2 position);
void BLB_Particle2D_Rotate(BLB_Particle2D *p, float angular_velocity);
void BLB_Particle2D_SetRotation(BLB_Particle2D *p, float rotation);
void BLB_Particle2D_Scale(BLB_Particle2D *p, HMM_Vec2 scale_velocity);
void BLB_Particle2D_SetScale(BLB_Particle2D *p, HMM_Vec2 scale);
void BLB_Particle2D_Transform(BLB_Particle2D *p, HMM_Vec2 position, float rotation, HMM_Vec2 scale);
int BLB_Particle2D_SetTexture(BLB_Particle2D *p, BLB_Texture *texture);
void BLB_Particle2D_SetMaterial(BLB_Particle2D *p, BLB_Material *material);
void BLB_Particle2D_FlipX(BLB_Particle2D *p);
void BLB_Particle2D_FlipY(BLB_Particle2D *p);

BLB_ParticleSystem2D *BLB_CreateParticleSystem2D(HMM_Vec2 particle_system_scale, HMM_Vec2 position, size_t count, size_t capacity, float lifetime,
                                                 float lifetime_variation, BLB_Texture *texture, bool screen_space);

void BLB_DestroyParticleSystem2D(BLB_ParticleSystem2D *ps);

size_t BLB_ParticleSystem2D_Emit(BLB_ParticleSystem2D *ps, size_t count);
bool BLB_ParticleSystem2D_SetCount(BLB_ParticleSystem2D *ps, size_t count);
void BLB_ParticleSystem2D_Clear(BLB_ParticleSystem2D *ps);

void BLB_ParticleSystem2D_Update(BLB_ParticleSystem2D *ps, float delta_time);

void BLB_ParticleSystem2D_Play(BLB_ParticleSystem2D *ps);
void BLB_ParticleSystem2D_Stop(BLB_ParticleSystem2D *ps);
void BLB_ParticleSystem2D_Restart(BLB_ParticleSystem2D *ps);

void BLB_ParticleSystem2D_Move(BLB_ParticleSystem2D *ps, HMM_Vec2 velocity);
void BLB_ParticleSystem2D_SetPosition(BLB_ParticleSystem2D *ps, HMM_Vec2 position);
void BLB_ParticleSystem2D_Rotate(BLB_ParticleSystem2D *ps, float angular_velocity);
void BLB_ParticleSystem2D_SetRotation(BLB_ParticleSystem2D *ps, float rotation);
void BLB_ParticleSystem2D_Scale(BLB_ParticleSystem2D *ps, HMM_Vec2 scale_velocity);
void BLB_ParticleSystem2D_SetScale(BLB_ParticleSystem2D *ps, HMM_Vec2 scale);
void BLB_ParticleSystem2D_Transform(BLB_ParticleSystem2D *ps, HMM_Vec2 position, float rotation, HMM_Vec2 scale);
int BLB_ParticleSystem2D_SetTexture(BLB_ParticleSystem2D *ps, BLB_Texture *texture);
void BLB_ParticleSystem2D_SetMaterial(BLB_ParticleSystem2D *ps, BLB_Material *material);
void BLB_ParticleSystem2D_FlipX(BLB_ParticleSystem2D *ps);
void BLB_ParticleSystem2D_FlipY(BLB_ParticleSystem2D *ps);

#endif
