#include "physics2d.h"

BLB_Collider2D *BLB_Physics2D_ColliderCreate(BLB_RigidBody2D *body, const BLB_Physics2DShape *shape) {
  if (!body || !shape || !b2Body_IsValid(body->handle))
    return NULL;

  BLB_Collider2D *collider = calloc(1, sizeof(*collider));
  if (!collider)
    return NULL;

  if (!blb_physics2d_copy_shape(&collider->shape, shape)) {
    blb_physics2d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  collider->body = body;
  collider->id = body->world->next_collider_id++;
  collider->filter.layer = 0;
  collider->filter.mask = UINT64_MAX;
  collider->filter.group = 0;
  collider->trigger = false;
  collider->active = true;
  collider->user_data = shape->user_data;

  b2ShapeDef def = b2DefaultShapeDef();
  def.filter = blb_physics2d_make_filter(collider);
  def.userData = collider->user_data;

  b2ShapeId handle = blb_physics2d_create_backend_shape(collider, &def);
  if (!b2Shape_IsValid(handle)) {
    blb_physics2d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  collider->handle = handle;
  collider->density = b2Shape_GetDensity(handle);
  collider->friction = b2Shape_GetFriction(handle);
  collider->restitution = b2Shape_GetRestitution(handle);

  b2Shape_SetUserData(handle, collider->user_data);

  BLB_Physics2DWorld *world = body->world;
  BLB_Box2DBackend *backend = blb_physics2d_get_backend(world);

  if (!blb_physics2d_append_body_collider(body, collider)) {
    b2DestroyShape(handle, true);
    blb_physics2d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  if (!blb_physics2d_append_collider(backend, collider, world->collider_count)) {
    blb_physics2d_remove_body_collider(body, collider);
    b2DestroyShape(handle, true);
    blb_physics2d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  ++world->collider_count;
  return collider;
}

void BLB_Physics2D_ColliderDestroy(BLB_Collider2D *collider) {
  if (!collider || !collider->body || !collider->body->world)
    return;

  BLB_Physics2DWorld *world = collider->body->world;
  BLB_Box2DBackend *backend = blb_physics2d_get_backend(world);

  if (b2Shape_IsValid(collider->handle))
    b2DestroyShape(collider->handle, true);

  blb_physics2d_remove_body_collider(collider->body, collider);
  blb_physics2d_remove_collider(backend, collider, world->collider_count);

  if (world->collider_count > 0)
    --world->collider_count;

  blb_physics2d_free_shape_copy(&collider->shape);
  free(collider);
}

BLB_Physics2DColliderId BLB_Physics2D_ColliderGetId(const BLB_Collider2D *collider) { return collider ? collider->id : 0; }

void BLB_Physics2D_ColliderSetFilter(BLB_Collider2D *collider, BLB_PhysicsFilter filter) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return;

  collider->filter = filter;
  b2Shape_SetFilter(collider->handle, blb_physics2d_make_filter(collider));
}

BLB_PhysicsFilter BLB_Physics2D_ColliderGetFilter(const BLB_Collider2D *collider) {
  BLB_PhysicsFilter filter = {0, 0, 0};
  if (collider)
    filter = collider->filter;
  return filter;
}

void BLB_Physics2D_ColliderSetLayer(BLB_Collider2D *collider, uint8_t layer) {
  if (!collider || layer >= 64)
    return;

  collider->filter.layer = layer;
  if (b2Shape_IsValid(collider->handle))
    b2Shape_SetFilter(collider->handle, blb_physics2d_make_filter(collider));
}

void BLB_Physics2D_ColliderSetMask(BLB_Collider2D *collider, BLB_PhysicsLayerMask mask) {
  if (!collider)
    return;

  collider->filter.mask = mask;
  if (b2Shape_IsValid(collider->handle))
    b2Shape_SetFilter(collider->handle, blb_physics2d_make_filter(collider));
}

void BLB_Physics2D_ColliderSetGroup(BLB_Collider2D *collider, int32_t group) {
  if (!collider)
    return;

  collider->filter.group = group;
  if (b2Shape_IsValid(collider->handle))
    b2Shape_SetFilter(collider->handle, blb_physics2d_make_filter(collider));
}

void BLB_Physics2D_ColliderSetDensity(BLB_Collider2D *collider, float density) {
  if (!collider || density < 0.0f || !b2Shape_IsValid(collider->handle))
    return;

  collider->density = density;
  b2Shape_SetDensity(collider->handle, density, true);
}

float BLB_Physics2D_ColliderGetDensity(const BLB_Collider2D *collider) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return 0.0f;

  return b2Shape_GetDensity(collider->handle);
}

void BLB_Physics2D_ColliderSetFriction(BLB_Collider2D *collider, float friction) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return;

  collider->friction = friction;
  b2Shape_SetFriction(collider->handle, friction);
}

float BLB_Physics2D_ColliderGetFriction(const BLB_Collider2D *collider) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return 0.0f;

  return b2Shape_GetFriction(collider->handle);
}

void BLB_Physics2D_ColliderSetRestitution(BLB_Collider2D *collider, float restitution) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return;

  collider->restitution = restitution;
  b2Shape_SetRestitution(collider->handle, restitution);
}

float BLB_Physics2D_ColliderGetRestitution(const BLB_Collider2D *collider) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return 0.0f;

  return b2Shape_GetRestitution(collider->handle);
}

void BLB_Physics2D_ColliderSetTrigger(BLB_Collider2D *collider, bool trigger) {
  if (!collider || collider->trigger == trigger)
    return;

  blb_physics2d_recreate_shape(collider, trigger);
}

bool BLB_Physics2D_ColliderIsTrigger(const BLB_Collider2D *collider) { return collider ? collider->trigger : false; }

void BLB_Physics2D_ColliderSetActive(BLB_Collider2D *collider, bool active) {
  if (!collider || !b2Shape_IsValid(collider->handle))
    return;

  collider->active = active;
  b2Shape_SetFilter(collider->handle, blb_physics2d_make_filter(collider));
}

bool BLB_Physics2D_ColliderIsActive(const BLB_Collider2D *collider) { return collider ? collider->active : false; }
