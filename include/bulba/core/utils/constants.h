#ifndef BLB_CONSTANTS_H
#define BLB_CONSTANTS_H

#include "bulba/core/math3v/HandmadeMath.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Math
extern const float BLB_PI;
extern const float BLB_TAU;
extern const float BLB_HALF_PI;
extern const float BLB_QUARTER_PI;

extern const float BLB_E;
extern const float BLB_SQRT2;
extern const float BLB_SQRT3;

extern const float BLB_DEG2RAD;
extern const float BLB_RAD2DEG;

extern const float BLB_EPSILON;
extern const float BLB_EPSILON_SMALL;
extern const float BLB_EPSILON_LARGE;

extern const float BLB_INFINITY;
extern const float BLB_NEGATIVE_INFINITY;

// Time
extern const float BLB_DEFAULT_FPS;
extern const float BLB_DEFAULT_DELTA_TIME;

// Physics
extern const float BLB_GRAVITY;
extern const float BLB_GRAVITY_X;
extern const float BLB_GRAVITY_Y;
extern const float BLB_GRAVITY_Z;

extern const float BLB_DEFAULT_PHYSICS_FPS;
extern const float BLB_DEFAULT_PHYSICS_DELTA_TIME;

extern const float BLB_DEFAULT_FRICTION;
extern const float BLB_DEFAULT_RESTITUTION;
extern const float BLB_DEFAULT_LINEAR_DAMPING;
extern const float BLB_DEFAULT_ANGULAR_DAMPING;

// Transform
extern const float BLB_DEFAULT_POSITION;
extern const float BLB_DEFAULT_ROTATION;
extern const float BLB_DEFAULT_SCALE;

// Camera
extern const float BLB_DEFAULT_FOV;
extern const float BLB_DEFAULT_NEAR_CLIP;
extern const float BLB_DEFAULT_FAR_CLIP;

extern const float BLB_DEFAULT_CAMERA_SPEED;
extern const float BLB_DEFAULT_CAMERA_SENSITIVITY;

// Rendering
extern const float BLB_DEFAULT_EXPOSURE;
extern const float BLB_DEFAULT_GAMMA;

extern const float BLB_DEFAULT_AMBIENT_STRENGTH;

extern const float BLB_DEFAULT_SHADOW_BIAS;

// Material
extern const float BLB_DEFAULT_METALLIC;
extern const float BLB_DEFAULT_ROUGHNESS;
extern const float BLB_DEFAULT_SPECULAR;
extern const float BLB_DEFAULT_IOR;
extern const float BLB_DEFAULT_TRANSMISSION;

extern const float BLB_DEFAULT_CLEARCOAT;
extern const float BLB_DEFAULT_SHEEN;
extern const float BLB_DEFAULT_IRIDESCENCE;
extern const float BLB_DEFAULT_ANISOTROPY;

extern const float BLB_DEFAULT_EMISSION;
extern const float BLB_DEFAULT_EMISSION_STRENGTH;

extern const float BLB_DEFAULT_ALPHA;

// Lighting
extern const float BLB_DEFAULT_LIGHT_INTENSITY;
extern const float BLB_DEFAULT_LIGHT_RANGE;

// Audio
extern const float BLB_DEFAULT_AUDIO_VOLUME;
extern const float BLB_DEFAULT_AUDIO_PITCH;

// Particles
extern const float BLB_DEFAULT_PARTICLE_LIFETIME;
extern const float BLB_DEFAULT_PARTICLE_SCALE;

// Colors
extern const HMM_Vec4 BLB_COLOR_BLACK;
extern const HMM_Vec4 BLB_COLOR_WHITE;

extern const HMM_Vec4 BLB_COLOR_RED;
extern const HMM_Vec4 BLB_COLOR_GREEN;
extern const HMM_Vec4 BLB_COLOR_BLUE;

extern const HMM_Vec4 BLB_COLOR_YELLOW;
extern const HMM_Vec4 BLB_COLOR_CYAN;
extern const HMM_Vec4 BLB_COLOR_MAGENTA;

extern const HMM_Vec4 BLB_COLOR_GRAY;

extern const HMM_Vec4 BLB_COLOR_TRANSPARENT;

// Common values
extern const float BLB_ZERO;
extern const float BLB_ONE;
extern const float BLB_HALF;

#ifdef __cplusplus
}
#endif

#endif
