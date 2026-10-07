#include "bulba/core/objects3d/objects3d.h"
#include "bulba/core/render/object_aggregation.h"

static void mark_transform_changed(BLB_Object3D *object) {
  if (!object)
    return;

  ++object->transform_revision;
  if (object->transform_revision == 0)
    object->transform_revision = 1;

  object->optimization_dynamic = true;
  object->optimization_transform_dirty = true;
  object->optimization_stable_frames = 0;
  object->optimization_static_recovered = false;

  if (object->optimization_cluster)
    object->optimization_cluster->dirty = true;
}

void BLB_Object3D_Move(BLB_Object3D *object, HMM_Vec3 velocity) {
  if (!object)
    return;

  if (object->delta_time)
    velocity = HMM_MulV3F(velocity, *object->delta_time);

  object->position = HMM_AddV3(object->position, velocity);
  mark_transform_changed(object);
}

void BLB_Object3D_SetPosition(BLB_Object3D *object, HMM_Vec3 position) {
  if (!object)
    return;

  object->position = position;
  mark_transform_changed(object);
}

void BLB_Object3D_Rotate(BLB_Object3D *object, HMM_Vec3 angular_velocity) {
  if (!object)
    return;

  if (object->delta_time)
    angular_velocity = HMM_MulV3F(angular_velocity, *object->delta_time);

  object->rotation = HMM_AddV3(object->rotation, angular_velocity);
  mark_transform_changed(object);
}

void BLB_Object3D_SetRotation(BLB_Object3D *object, HMM_Vec3 rotation) {
  if (!object)
    return;

  object->rotation = rotation;
  mark_transform_changed(object);
}

void BLB_Object3D_Scale(BLB_Object3D *object, HMM_Vec3 scale_velocity) {
  if (!object)
    return;

  if (object->delta_time)
    scale_velocity = HMM_MulV3F(scale_velocity, *object->delta_time);

  object->scale = HMM_AddV3(object->scale, scale_velocity);
  mark_transform_changed(object);
}

void BLB_Object3D_SetScale(BLB_Object3D *object, HMM_Vec3 scale) {
  if (!object)
    return;

  object->scale = scale;
  mark_transform_changed(object);
}

void BLB_Object3D_Transform(BLB_Object3D *object, HMM_Vec3 position, HMM_Vec3 rotation, HMM_Vec3 scale) {
  if (!object)
    return;

  object->position = position;
  object->rotation = rotation;
  object->scale = scale;
  mark_transform_changed(object);
}

void BLB_Object3D_SetMaterial(BLB_Object3D *object, BLB_Material *material) {
  if (!object || object->material == material)
    return;

  if (material)
    BLB_Material_Retain(material);

  if (object->material)
    BLB_Material_Release(object->material);

  object->material = material;
}

int BLB_Object3D_SetTexture(BLB_Object3D *object, BLB_Texture *texture) {
  if (!object)
    return -1;

  if (texture && texture->type == BLB_TEXTURE_CUBE)
    return -1;

  if (object->texture == texture)
    return 0;

  if (texture)
    BLB_Texture_Retain(texture);

  if (object->texture)
    BLB_Texture_Release(object->texture);

  object->texture = texture;
  return 0;
}

void BLB_Object3D_FlipX(BLB_Object3D *object) {
  if (!object)
    return;

  object->scale.x *= -1.0f;
  mark_transform_changed(object);
}

void BLB_Object3D_FlipY(BLB_Object3D *object) {
  if (!object)
    return;

  object->scale.y *= -1.0f;
  mark_transform_changed(object);
}

void BLB_Object3D_FlipZ(BLB_Object3D *object) {
  if (!object)
    return;

  object->scale.z *= -1.0f;
  mark_transform_changed(object);
}

void BLB_Object3D_SetHandler(BLB_Object3D *object, const BLB_ObjectHandler *handler) {
  if (!object)
    return;

  if (!handler) {
    BLB_Object3D_ClearHandler(object);
    return;
  }

  if (!object->handler) {
    object->handler = malloc(sizeof(*object->handler));
    if (!object->handler)
      return;
  }

  *object->handler = *handler;
  object->handler->captured_buttons = 0;
  object->handler->hovered = false;
  object->handler->enabled = true;
}

void BLB_Object3D_ClearHandler(BLB_Object3D *object) {
  if (!object)
    return;
  free(object->handler);
  object->handler = NULL;
}
