#ifndef BULBA_CORE_OBJECTS3D_SKYBOX_H
#define BULBA_CORE_OBJECTS3D_SKYBOX_H

#include "bulba/core/objects3d/objects3d.h"
#include "bulba/core/render/texture.h"

#include <stdint.h>

typedef struct BLB_Skybox {
  BLB_Object3D *object;
  float fov_scale;
} BLB_Skybox;

BLB_Skybox *BLB_Skybox_Create(BLB_Texture *texture);
BLB_Skybox *BLB_Skybox_Load2D(const char *path);
BLB_Skybox *BLB_Skybox_Load3D(const char *path, uint32_t width, uint32_t height, uint32_t depth);
void BLB_Skybox_Destroy(BLB_Skybox *skybox);

void BLB_Skybox_SetFovScale(BLB_Skybox *skybox, float fov_scale);

#endif
