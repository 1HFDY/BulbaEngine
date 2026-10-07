#include "physics3d.h"

void blb_physics3d_free_shape_copy(BLB_Physics3DShape *shape) {
  if (!shape)
    return;

  if (shape->type == BLB_PHYSICS3D_SHAPE_CONVEX_HULL) {
    free(shape->data.convex_hull.vertices);
    shape->data.convex_hull.vertices = NULL;
    shape->data.convex_hull.vertex_count = 0;
  } else if (shape->type == BLB_PHYSICS3D_SHAPE_TRIANGLE_MESH) {
    free(shape->data.triangle_mesh.vertices);
    free(shape->data.triangle_mesh.indices);
    shape->data.triangle_mesh.vertices = NULL;
    shape->data.triangle_mesh.indices = NULL;
    shape->data.triangle_mesh.vertex_count = 0;
    shape->data.triangle_mesh.index_count = 0;
  }
}

bool blb_physics3d_copy_shape(BLB_Physics3DShape *dst, const BLB_Physics3DShape *src) {
  if (!dst || !src)
    return false;

  *dst = *src;

  if (src->type == BLB_PHYSICS3D_SHAPE_CONVEX_HULL) {
    if (!src->data.convex_hull.vertices || src->data.convex_hull.vertex_count < 4)
      return false;

    dst->data.convex_hull.vertices = malloc(src->data.convex_hull.vertex_count * sizeof(*dst->data.convex_hull.vertices));
    if (!dst->data.convex_hull.vertices)
      return false;

    memcpy(dst->data.convex_hull.vertices, src->data.convex_hull.vertices,
           src->data.convex_hull.vertex_count * sizeof(*dst->data.convex_hull.vertices));
  } else if (src->type == BLB_PHYSICS3D_SHAPE_TRIANGLE_MESH) {
    if (!src->data.triangle_mesh.vertices || !src->data.triangle_mesh.indices || src->data.triangle_mesh.vertex_count < 3 ||
        src->data.triangle_mesh.index_count < 3)
      return false;

    dst->data.triangle_mesh.vertices = malloc(src->data.triangle_mesh.vertex_count * sizeof(*dst->data.triangle_mesh.vertices));
    if (!dst->data.triangle_mesh.vertices)
      return false;

    dst->data.triangle_mesh.indices = malloc(src->data.triangle_mesh.index_count * sizeof(*dst->data.triangle_mesh.indices));
    if (!dst->data.triangle_mesh.indices) {
      free(dst->data.triangle_mesh.vertices);
      dst->data.triangle_mesh.vertices = NULL;
      return false;
    }

    memcpy(dst->data.triangle_mesh.vertices, src->data.triangle_mesh.vertices,
           src->data.triangle_mesh.vertex_count * sizeof(*dst->data.triangle_mesh.vertices));
    memcpy(dst->data.triangle_mesh.indices, src->data.triangle_mesh.indices,
           src->data.triangle_mesh.index_count * sizeof(*dst->data.triangle_mesh.indices));
  }

  return true;
}

b3ShapeId blb_physics3d_create_backend_shape(const BLB_Collider3D *collider, const b3ShapeDef *def) {
  if (!collider || !def)
    return b3_nullShapeId;

  b3BodyId body = collider->body->handle;

  switch (collider->shape.type) {
  case BLB_PHYSICS3D_SHAPE_BOX: {
    HMM_Vec3 h = collider->shape.data.box.half_extents;
    b3BoxHull box = b3MakeBoxHull(h.x, h.y, h.z);
    return b3CreateHullShape(body, def, &box.base);
  }

  case BLB_PHYSICS3D_SHAPE_SPHERE: {
    b3Sphere sphere = {
        .center = {0.0f, 0.0f, 0.0f},
        .radius = collider->shape.data.sphere.radius,
    };
    return b3CreateSphereShape(body, def, &sphere);
  }

  case BLB_PHYSICS3D_SHAPE_CAPSULE: {
    b3Capsule capsule = {
        .center1 = {0.0f, -collider->shape.data.capsule.half_height, 0.0f},
        .center2 = {0.0f, collider->shape.data.capsule.half_height, 0.0f},
        .radius = collider->shape.data.capsule.radius,
    };
    return b3CreateCapsuleShape(body, def, &capsule);
  }

  case BLB_PHYSICS3D_SHAPE_CONVEX_HULL: {
    if (!collider->shape.data.convex_hull.vertices || collider->shape.data.convex_hull.vertex_count < 4)
      return b3_nullShapeId;

    b3HullData *hull = b3CreateHull((const b3Vec3 *)collider->shape.data.convex_hull.vertices, (int)collider->shape.data.convex_hull.vertex_count,
                                    (int)collider->shape.data.convex_hull.vertex_count);

    if (!hull)
      return b3_nullShapeId;

    b3ShapeId result = b3CreateHullShape(body, def, hull);
    b3DestroyHull(hull);
    return result;
  }

  case BLB_PHYSICS3D_SHAPE_PLANE:
  case BLB_PHYSICS3D_SHAPE_TRIANGLE_MESH:
  case BLB_PHYSICS3D_SHAPE_COMPOUND:
  default:
    return b3_nullShapeId;
  }
}

bool blb_physics3d_recreate_shape(BLB_Collider3D *collider, bool trigger) {
  if (!collider || !b3Shape_IsValid(collider->handle))
    return false;

  b3ShapeDef def = b3DefaultShapeDef();
  def.density = collider->density;
  def.baseMaterial.friction = collider->friction;
  def.baseMaterial.restitution = collider->restitution;
  def.filter = blb_physics3d_make_filter(collider);
  def.isSensor = trigger;
  def.updateBodyMass = false;

  b3ShapeId replacement = blb_physics3d_create_backend_shape(collider, &def);
  if (!b3Shape_IsValid(replacement))
    return false;

  b3ShapeId old = collider->handle;
  collider->handle = replacement;
  collider->trigger = trigger;

  b3Shape_SetUserData(replacement, collider->user_data);
  b3DestroyShape(old, false);
  b3Body_ApplyMassFromShapes(collider->body->handle);

  return true;
}
