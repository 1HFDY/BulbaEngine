#include "physics2d.h"

BLB_RigidBody2D *BLB_Physics2D_BodyCreate(BLB_Physics2DWorld *world, BLB_Physics2DBodyType type) {
  if (!blb_physics2d_valid_world(world))
    return NULL;

  BLB_Box2DBackend *backend = blb_physics2d_get_backend(world);
  b2BodyDef def = b2DefaultBodyDef();
  def.type = blb_physics2d_to_b2_body_type(type);

  b2BodyId handle = b2CreateBody(backend->world_id, &def);
  if (!b2Body_IsValid(handle))
    return NULL;

  BLB_RigidBody2D *body = calloc(1, sizeof(*body));
  if (!body) {
    b2DestroyBody(handle);
    return NULL;
  }

  body->id = world->next_body_id++;
  body->handle = handle;
  body->world = world;
  body->type = type;
  body->user_data = NULL;

  b2Body_SetUserData(handle, body);

  if (!blb_physics2d_append_body(backend, body, world->body_count)) {
    b2DestroyBody(handle);
    free(body);
    return NULL;
  }

  ++world->body_count;
  return body;
}


void BLB_Physics2D_BodyDestroy(BLB_Physics2DWorld *world, BLB_RigidBody2D *body) {
  if (!world || !body || body->world != world)
    return;

  BLB_Box2DBackend *backend = blb_physics2d_get_backend(world);

  if (b2Body_IsValid(body->handle))
    b2DestroyBody(body->handle);

  while (body->collider_count > 0) {
    BLB_Collider2D *collider = body->colliders[body->collider_count - 1];
    blb_physics2d_remove_collider(backend, collider, world->collider_count);
    if (world->collider_count > 0)
      --world->collider_count;
    blb_physics2d_free_shape_copy(&collider->shape);
    free(collider);
    --body->collider_count;
  }

  free(body->colliders);
  body->colliders = NULL;

  blb_physics2d_remove_body(backend, body, world->body_count);
  if (world->body_count > 0)
    --world->body_count;

  free(body);
}


BLB_Physics2DBodyId BLB_Physics2D_BodyGetId(const BLB_RigidBody2D *body) { return body ? body->id : 0; }


void BLB_Physics2D_BodySetType(BLB_RigidBody2D *body, BLB_Physics2DBodyType type) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Body_SetType(body->handle, blb_physics2d_to_b2_body_type(type));
  body->type = type;
}


BLB_Physics2DBodyType BLB_Physics2D_BodyGetType(const BLB_RigidBody2D *body) {
  if (!body || !b2Body_IsValid(body->handle))
    return BLB_PHYSICS2D_STATIC;

  return blb_physics2d_from_b2_body_type(b2Body_GetType(body->handle));
}


void BLB_Physics2D_BodySetPosition(BLB_RigidBody2D *body, HMM_Vec2 position) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Rot rotation = b2Body_GetRotation(body->handle);
  b2Body_SetTransform(body->handle, (b2Vec2){position.x, position.y}, rotation);
}


HMM_Vec2 BLB_Physics2D_BodyGetPosition(const BLB_RigidBody2D *body) {
  if (!body || !b2Body_IsValid(body->handle))
    return HMM_V2(0.0f, 0.0f);

  b2Vec2 position = b2Body_GetPosition(body->handle);
  return HMM_V2(position.x, position.y);
}


void BLB_Physics2D_BodySetRotation(BLB_RigidBody2D *body, float rotation) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Vec2 position = b2Body_GetPosition(body->handle);
  b2Body_SetTransform(body->handle, position, b2MakeRot(rotation));
}


float BLB_Physics2D_BodyGetRotation(const BLB_RigidBody2D *body) {
  if (!body || !b2Body_IsValid(body->handle))
    return 0.0f;

  b2Rot rotation = b2Body_GetRotation(body->handle);
  return atan2f(rotation.s, rotation.c);
}


void BLB_Physics2D_BodySetVelocity(BLB_RigidBody2D *body, HMM_Vec2 velocity) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Body_SetLinearVelocity(body->handle, (b2Vec2){velocity.x, velocity.y});
}


HMM_Vec2 BLB_Physics2D_BodyGetVelocity(const BLB_RigidBody2D *body) {
  if (!body || !b2Body_IsValid(body->handle))
    return HMM_V2(0.0f, 0.0f);

  b2Vec2 velocity = b2Body_GetLinearVelocity(body->handle);
  return HMM_V2(velocity.x, velocity.y);
}


void BLB_Physics2D_BodyApplyForce(BLB_RigidBody2D *body, HMM_Vec2 force) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Body_ApplyForceToCenter(body->handle, (b2Vec2){force.x, force.y}, true);
}


void BLB_Physics2D_BodyApplyImpulse(BLB_RigidBody2D *body, HMM_Vec2 impulse) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  b2Body_ApplyLinearImpulseToCenter(body->handle, (b2Vec2){impulse.x, impulse.y}, true);
}


void BLB_Physics2D_BodySetGravityScale(BLB_RigidBody2D *body, float scale) {
  if (body && b2Body_IsValid(body->handle))
    b2Body_SetGravityScale(body->handle, scale);
}


void BLB_Physics2D_BodySetLinearDamping(BLB_RigidBody2D *body, float damping) {
  if (body && b2Body_IsValid(body->handle))
    b2Body_SetLinearDamping(body->handle, damping);
}


void BLB_Physics2D_BodySetAngularDamping(BLB_RigidBody2D *body, float damping) {
  if (body && b2Body_IsValid(body->handle))
    b2Body_SetAngularDamping(body->handle, damping);
}


void BLB_Physics2D_BodySetActive(BLB_RigidBody2D *body, bool active) {
  if (!body || !b2Body_IsValid(body->handle))
    return;

  if (active)
    b2Body_Enable(body->handle);
  else
    b2Body_Disable(body->handle);
}


bool BLB_Physics2D_BodyIsActive(const BLB_RigidBody2D *body) { return body && b2Body_IsValid(body->handle) && b2Body_IsEnabled(body->handle); }

