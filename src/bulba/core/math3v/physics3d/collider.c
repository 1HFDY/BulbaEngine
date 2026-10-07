#include "physics3d.h"

BLB_Collider3D *BLB_Physics3D_ColliderCreate(BLB_RigidBody3D *body, const BLB_Physics3DShape *shape) {
  if (!body || !shape || !b3Body_IsValid(body->handle))
    return NULL;

  BLB_Collider3D *collider = calloc(1, sizeof(*collider));
  if (!collider)
    return NULL;

  if (!blb_physics3d_copy_shape(&collider->shape, shape)) {
    blb_physics3d_free_shape_copy(&collider->shape);
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

  b3ShapeDef def = b3DefaultShapeDef();
  def.filter = blb_physics3d_make_filter(collider);
  def.userData = collider->user_data;

  b3ShapeId handle = blb_physics3d_create_backend_shape(collider, &def);
  if (!b3Shape_IsValid(handle)) {
    blb_physics3d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  collider->handle = handle;
  collider->density = b3Shape_GetDensity(handle);
  collider->friction = b3Shape_GetFriction(handle);
  collider->restitution = b3Shape_GetRestitution(handle);

  b3Shape_SetUserData(handle, collider->user_data);

  BLB_Physics3DWorld *world = body->world;
  BLB_Box3DBackend *backend = blb_physics3d_get_backend(world);

  if (!blb_physics3d_append_body_collider(body, collider)) {
    b3DestroyShape(handle, true);
    blb_physics3d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  if (!blb_physics3d_append_collider(backend, collider, world->collider_count)) {
    blb_physics3d_remove_body_collider(body, collider);
    b3DestroyShape(handle, true);
    blb_physics3d_free_shape_copy(&collider->shape);
    free(collider);
    return NULL;
  }

  ++world->collider_count;
  return collider;
}

void BLB_Physics3D_ColliderDestroy(BLB_Collider3D *collider) {
  if (!collider || !collider->body || !collider->body->world)
    return;

  BLB_Physics3DWorld *world = collider->body->world;
  BLB_Box3DBackend *backend = blb_physics3d_get_backend(world);

  if (b3Shape_IsValid(collider->handle))
    b3DestroyShape(collider->handle, true);

  blb_physics3d_remove_body_collider(collider->body, collider);
  blb_physics3d_remove_collider(backend, collider, world->collider_count);

  if (world->collider_count > 0)
    --world->collider_count;

  blb_physics3d_free_shape_copy(&collider->shape);
  free(collider);
}

BLB_Physics3DColliderId BLB_Physics3D_ColliderGetId(const BLB_Collider3D *collider) { return collider ? collider->id : 0; }

void BLB_Physics3D_ColliderSetFilter(BLB_Collider3D *collider, BLB_PhysicsFilter filter) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return;

  collider->filter = filter;
  b3Shape_SetFilter(collider->handle, blb_physics3d_make_filter(collider), true);
}

BLB_PhysicsFilter BLB_Physics3D_ColliderGetFilter(const BLB_Collider3D *collider) {
  BLB_PhysicsFilter filter = {0, 0, 0};
  if (collider)
    filter = collider->filter;
  return filter;
}

void BLB_Physics3D_ColliderSetLayer(BLB_Collider3D *collider, uint8_t layer) {
  if (!collider || layer >= 64)
    return;

  collider->filter.layer = layer;
  if (b3Shape_IsValid(collider->handle))
    b3Shape_SetFilter(collider->handle, blb_physics3d_make_filter(collider), true);
}

void BLB_Physics3D_ColliderSetMask(BLB_Collider3D *collider, BLB_PhysicsLayerMask mask) {
  if (!collider)
    return;

  collider->filter.mask = mask;
  if (b3Shape_IsValid(collider->handle))
    b3Shape_SetFilter(collider->handle, blb_physics3d_make_filter(collider), true);
}

void BLB_Physics3D_ColliderSetGroup(BLB_Collider3D *collider, int32_t group) {
  if (!collider)
    return;

  collider->filter.group = group;
  if (b3Shape_IsValid(collider->handle))
    b3Shape_SetFilter(collider->handle, blb_physics3d_make_filter(collider), true);
}

void BLB_Physics3D_ColliderSetDensity(BLB_Collider3D *collider, float density) {
  if (!collider || density < 0.0f || !b3Shape_IsValid(collider->handle))
    return;

  collider->density = density;
  b3Shape_SetDensity(collider->handle, density, true);
}

float BLB_Physics3D_ColliderGetDensity(const BLB_Collider3D *collider) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return 0.0f;

  return b3Shape_GetDensity(collider->handle);
}

const BLB_Physics3DShape *BLB_Physics3D_ColliderGetShape(const BLB_Collider3D *collider) { return collider ? &collider->shape : NULL; }

void BLB_Physics3D_ColliderSetFriction(BLB_Collider3D *collider, float friction) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return;

  collider->friction = friction;
  b3Shape_SetFriction(collider->handle, friction);
}

float BLB_Physics3D_ColliderGetFriction(const BLB_Collider3D *collider) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return 0.0f;

  return b3Shape_GetFriction(collider->handle);
}

void BLB_Physics3D_ColliderSetRestitution(BLB_Collider3D *collider, float restitution) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return;

  collider->restitution = restitution;
  b3Shape_SetRestitution(collider->handle, restitution);
}

float BLB_Physics3D_ColliderGetRestitution(const BLB_Collider3D *collider) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return 0.0f;

  return b3Shape_GetRestitution(collider->handle);
}

void BLB_Physics3D_ColliderSetTrigger(BLB_Collider3D *collider, bool trigger) {
  if (!collider || collider->trigger == trigger)
    return;

  blb_physics3d_recreate_shape(collider, trigger);
}

bool BLB_Physics3D_ColliderIsTrigger(const BLB_Collider3D *collider) { return collider ? collider->trigger : false; }

void BLB_Physics3D_ColliderSetActive(BLB_Collider3D *collider, bool active) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return;

  collider->active = active;
  b3Shape_SetFilter(collider->handle, blb_physics3d_make_filter(collider), true);
}

bool BLB_Physics3D_ColliderIsActive(const BLB_Collider3D *collider) { return collider ? collider->active : false; }
