#ifndef BLB_HANDLERS_OBJECT_H
#define BLB_HANDLERS_OBJECT_H

#include "bulba/core/math3v/HandmadeMath.h"

#include <stdbool.h>
#include <stdint.h>

struct BLB_Object2D;
struct BLB_Object3D;

typedef enum { BLB_OBJECT_HANDLER_CLICK = 0, BLB_OBJECT_HANDLER_HOLD = 1 } BLB_ObjectHandlerEventType;

typedef enum { BLB_OBJECT_HANDLER_2D = 1, BLB_OBJECT_HANDLER_3D = 2 } BLB_ObjectHandlerObjectType;

typedef struct {
  BLB_ObjectHandlerEventType type;
  BLB_ObjectHandlerObjectType object_type;

  union {
    struct BLB_Object2D *object2d;
    struct BLB_Object3D *object3d;
    void *ptr;
  } object;

  int button;
  uint32_t mods;

  uint64_t frame;
  double time;

  double mouse_x;
  double mouse_y;

  double mouse_dx;
  double mouse_dy;

  HMM_Vec2 world_position;

  HMM_Vec3 ray_origin;
  HMM_Vec3 ray_direction;

  HMM_Vec3 hit_point;
  HMM_Vec3 hit_normal;

  float distance;
  float t;

  bool hovered;
  bool captured;
  bool inside;
} BLB_ObjectEvent;

typedef void (*BLB_ObjectHandlerCallback)(const BLB_ObjectEvent *event, void *userdata);

typedef struct {
  BLB_ObjectHandlerCallback on_click;
  BLB_ObjectHandlerCallback on_hold;

  void *userdata;

  const void *object_geometry;

  HMM_Vec3 object_bounds_min;
  HMM_Vec3 object_bounds_max;

  uint16_t captured_buttons;

  bool hovered;
  bool enabled;
} BLB_ObjectHandler;

void BLB_ObjectHandlerInit(BLB_ObjectHandler *handler);
BLB_ObjectEvent BLB_ObjectEventZero(void);

#endif
