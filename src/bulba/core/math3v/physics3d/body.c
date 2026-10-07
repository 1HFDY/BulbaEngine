#include "physics3d.h"

static b3Quat to_b3_quat(HMM_Quat rotation) {
  b3Quat result;
  result.v.x = rotation.x;
  result.v.y = rotation.y;
  result.v.z = rotation.z;
  result.s = rotation.w;
  return result;
}

static HMM_Quat from_b3_quat(b3Quat rotation) {
  HMM_Quat result;
  result.x = rotation.v.x;
  result.y = rotation.v.y;
  result.z = rotation.v.z;
  result.w = rotation.s;
  return result;
}

BLB_RigidBody3D *BLB_Physics3D_BodyCreate(BLB_Physics3DWorld *world, BLB_Physics3DBodyType type) {
  if (!blb_physics3d_valid_world(world))
    return NULL;

  BLB_Box3DBackend *backend = blb_physics3d_get_backend(world);
  b3BodyDef def = b3DefaultBodyDef();
  def.type = blb_physics3d_to_b3_body_type(type);

  b3BodyId handle = b3CreateBody(backend->world_id, &def);
  if (!b3Body_IsValid(handle))
    return NULL;

  BLB_RigidBody3D *body = calloc(1, sizeof(*body));
  if (!body) {
    b3DestroyBody(handle);
    return NULL;
  }

  body->id = world->next_body_id++;
  body->handle = handle;
  body->world = world;
  body->type = type;
  body->user_data = NULL;

  b3Body_SetUserData(handle, body);

  if (!blb_physics3d_append_body(backend, body, world->body_count)) {
    b3DestroyBody(handle);
    free(body);
    return NULL;
  }

  ++world->body_count;
  return body;
}

void BLB_Physics3D_BodyDestroy(BLB_Physics3DWorld *world, BLB_RigidBody3D *body) {
  if (!world || !body || body->world != world)
    return;

  BLB_Box3DBackend *backend = blb_physics3d_get_backend(world);

  if (b3Body_IsValid(body->handle))
    b3DestroyBody(body->handle);

  while (body->collider_count > 0) {
    BLB_Collider3D *collider = body->colliders[body->collider_count - 1];
    blb_physics3d_remove_collider(backend, collider, world->collider_count);
    if (world->collider_count > 0)
      --world->collider_count;
    blb_physics3d_free_shape_copy(&collider->shape);
    free(collider);
    --body->collider_count;
  }

  free(body->colliders);
  body->colliders = NULL;

  blb_physics3d_remove_body(backend, body, world->body_count);
  if (world->body_count > 0)
    --world->body_count;

  free(body);
}

BLB_Physics3DBodyId BLB_Physics3D_BodyGetId(const BLB_RigidBody3D *body) { return body ? body->id : 0; }

void BLB_Physics3D_BodySetType(BLB_RigidBody3D *body, BLB_Physics3DBodyType type) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Body_SetType(body->handle, blb_physics3d_to_b3_body_type(type));
  body->type = type;
}

BLB_Physics3DBodyType BLB_Physics3D_BodyGetType(const BLB_RigidBody3D *body) {
  if (!body || !b3Body_IsValid(body->handle))
    return BLB_PHYSICS3D_STATIC;

  return blb_physics3d_from_b3_body_type(b3Body_GetType(body->handle));
}

void BLB_Physics3D_BodySetPosition(BLB_RigidBody3D *body, HMM_Vec3 position) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Quat rotation = b3Body_GetRotation(body->handle);
  b3Body_SetTransform(body->handle, (b3Pos){position.x, position.y, position.z}, rotation);
}

HMM_Vec3 BLB_Physics3D_BodyGetPosition(const BLB_RigidBody3D *body) {
  if (!body || !b3Body_IsValid(body->handle))
    return HMM_V3(0.0f, 0.0f, 0.0f);

  b3Pos position = b3Body_GetPosition(body->handle);
  return HMM_V3((float)position.x, (float)position.y, (float)position.z);
}

void BLB_Physics3D_BodySetRotation(BLB_RigidBody3D *body, HMM_Quat rotation) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Pos position = b3Body_GetPosition(body->handle);
  b3Body_SetTransform(body->handle, position, to_b3_quat(rotation));
}

HMM_Quat BLB_Physics3D_BodyGetRotation(const BLB_RigidBody3D *body) {
  if (!body || !b3Body_IsValid(body->handle)) {
    HMM_Quat identity = {0};
    identity.w = 1.0f;
    return identity;
  }

  return from_b3_quat(b3Body_GetRotation(body->handle));
}

void BLB_Physics3D_BodySetVelocity(BLB_RigidBody3D *body, HMM_Vec3 velocity) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Body_SetLinearVelocity(body->handle, (b3Vec3){velocity.x, velocity.y, velocity.z});
}

HMM_Vec3 BLB_Physics3D_BodyGetVelocity(const BLB_RigidBody3D *body) {
  if (!body || !b3Body_IsValid(body->handle))
    return HMM_V3(0.0f, 0.0f, 0.0f);

  b3Vec3 velocity = b3Body_GetLinearVelocity(body->handle);
  return HMM_V3(velocity.x, velocity.y, velocity.z);
}

void BLB_Physics3D_BodySetAngularVelocity(BLB_RigidBody3D *body, HMM_Vec3 velocity) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Body_SetAngularVelocity(body->handle, (b3Vec3){velocity.x, velocity.y, velocity.z});
}

HMM_Vec3 BLB_Physics3D_BodyGetAngularVelocity(const BLB_RigidBody3D *body) {
  if (!body || !b3Body_IsValid(body->handle))
    return HMM_V3(0.0f, 0.0f, 0.0f);

  b3Vec3 velocity = b3Body_GetAngularVelocity(body->handle);
  return HMM_V3(velocity.x, velocity.y, velocity.z);
}

void BLB_Physics3D_BodyApplyForce(BLB_RigidBody3D *body, HMM_Vec3 force) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Body_ApplyForceToCenter(body->handle, (b3Vec3){force.x, force.y, force.z}, true);
}

void BLB_Physics3D_BodyApplyImpulse(BLB_RigidBody3D *body, HMM_Vec3 impulse) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  b3Body_ApplyLinearImpulseToCenter(body->handle, (b3Vec3){impulse.x, impulse.y, impulse.z}, true);
}

void BLB_Physics3D_BodySetGravityScale(BLB_RigidBody3D *body, float scale) {
  if (body && b3Body_IsValid(body->handle))
    b3Body_SetGravityScale(body->handle, scale);
}

void BLB_Physics3D_BodySetLinearDamping(BLB_RigidBody3D *body, float damping) {
  if (body && b3Body_IsValid(body->handle))
    b3Body_SetLinearDamping(body->handle, damping);
}

void BLB_Physics3D_BodySetAngularDamping(BLB_RigidBody3D *body, float damping) {
  if (body && b3Body_IsValid(body->handle))
    b3Body_SetAngularDamping(body->handle, damping);
}

void BLB_Physics3D_BodySetActive(BLB_RigidBody3D *body, bool active) {
  if (!body || !b3Body_IsValid(body->handle))
    return;

  if (active)
    b3Body_Enable(body->handle);
  else
    b3Body_Disable(body->handle);
}

bool BLB_Physics3D_BodyIsActive(const BLB_RigidBody3D *body) { return body && b3Body_IsValid(body->handle) && b3Body_IsEnabled(body->handle); }
