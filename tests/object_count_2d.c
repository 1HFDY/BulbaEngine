#include "bulba/core/objects2d/circle.h"
#include "bulba/core/objects2d/square.h"
#include "test_common.h"
#include "tests.h"

#include <math.h>

static BLB_Object2D *create_object(BLB_TestObject2DType type, HMM_Vec2 position) {
  switch (type) {
  case BLB_TEST_OBJECT2D_SQUARE:
    return BLB_CreateSquare2D(HMM_V2(1.8f, 1.8f), position, NULL, false);
  case BLB_TEST_OBJECT2D_CIRCLE:
    return BLB_CreateCircle2D(HMM_V2(1.8f, 1.8f), position, 6, NULL, false);
  default:
    return NULL;
  }
}

static void destroy_object(BLB_TestObject2DType type, BLB_Object2D *object) {
  if (!object)
    return;
  if (type == BLB_TEST_OBJECT2D_SQUARE)
    BLB_DestroySquare2D(object);
  else if (type == BLB_TEST_OBJECT2D_CIRCLE)
    BLB_DestroyCircle2D(object);
}

static void fit_camera_2d(BLB_TestContext *app, size_t columns, size_t rows, float spacing, float radius) {
  if (!app || !app->camera || !columns || !rows)
    return;
  float width = (float)(columns - 1) * spacing + radius * 2.0f;
  float height = (float)(rows - 1) * spacing + radius * 2.0f;
  float aspect = 1.0f;
  uint32_t viewport_width = 0;
  uint32_t viewport_height = 0;
  BLB_RendererGetViewport(&app->renderer, &viewport_width, &viewport_height);
  if (viewport_height > 0)
    aspect = (float)viewport_width / (float)viewport_height;
  if (aspect <= 0.0f)
    aspect = 1.0f;
  float tan_vertical = tanf(app->camera->fov * 0.5f);
  float tan_horizontal = tan_vertical * aspect;
  float distance_vertical = height * 0.5f / fmaxf(tan_vertical, 0.0001f);
  float distance_horizontal = width * 0.5f / fmaxf(tan_horizontal, 0.0001f);
  float distance = fmaxf(distance_vertical, distance_horizontal) * 1.15f + radius + 4.0f;
  BLB_Camera_SetPosition(app->camera, HMM_V3(0.0f, 0.0f, distance));
  app->camera->far_plane = fmaxf(1000.0f, distance * 4.0f);
}

int BLB_TestObjectCount2D(size_t count, BLB_TestObject2DType type, bool r) {
  if (count == 0 || (type != BLB_TEST_OBJECT2D_SQUARE && type != BLB_TEST_OBJECT2D_CIRCLE))
    return -1;

  BLB_TestContext app;
  if (BLB_TestContext_Init(&app, 1200, 900, "BulbaEngine - Object Count 2D", HMM_V3(0.0f, 0.0f, 30.0f), 6, 7, 11) != 0)
    return -1;

  BLB_Material *shared_material = BLB_Material_Build(BLB_MATERIAL_2D, NULL);
  if (!shared_material) {
    BLB_TestContext_Shutdown(&app);
    return -1;
  }
  BLB_Material_SetLighting(shared_material, false);
  BLB_Material_SetUnlit(shared_material, true);
  BLB_Material_SetDepth(shared_material, false, false);
  BLB_Material_SetDoubleSided(shared_material, true);

  BLB_Object2D **objects = calloc(count, sizeof(*objects));
  if (!objects) {
    BLB_Material_Release(shared_material);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  size_t columns = (size_t)ceil(sqrt((double)count * (1200.0 / 900.0)));
  if (columns < 1)
    columns = 1;
  size_t rows = (count + columns - 1) / columns;
  const float spacing = 2.2f;
  const float radius = 0.9f;
  int error = 0;

  for (size_t i = 0; i < count; ++i) {
    size_t row = i / columns;
    size_t column = i % columns;
    float x = ((float)column - (float)(columns - 1) * 0.5f) * spacing;
    float y = ((float)(rows - 1) * 0.5f - (float)row) * spacing;
    objects[i] = create_object(type, HMM_V2(x, y));
    if (!objects[i]) {
      error = 1;
      break;
    }
    BLB_Object2D_SetMaterial(objects[i], shared_material);
    objects[i]->layer = 1;
    objects[i]->color[0] = 120 + (unsigned char)(i % 120);
    objects[i]->color[1] = 170;
    objects[i]->color[2] = 230;
    if (BLB_AddObject2D(app.scene, objects[i]) != 0) {
      error = 1;
      break;
    }
  }

  BLB_Material_Release(shared_material);

  if (error) {
    for (size_t i = 0; i < count; ++i)
      destroy_object(type, objects[i]);
    free(objects);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  fit_camera_2d(&app, columns, rows, spacing, radius);

  while (!BLB_WindowShouldClose(app.window)) {
    float delta_time = 0.0f;
    int frame = BLB_TestContext_BeginFrame(&app, &delta_time);
    if (frame < 0)
      break;
    if (frame > 0)
      continue;
    if (r) {
      for (size_t i = 0; i < count; ++i)
        BLB_Object2D_Rotate(objects[i], 90.0f);
    }
    fit_camera_2d(&app, columns, rows, spacing, radius);
    if (BLB_TestContext_Draw(&app) < 0)
      break;
  }

  for (size_t i = 0; i < count; ++i) {
    BLB_RemoveObject2D(app.scene, objects[i]);
    destroy_object(type, objects[i]);
  }
  free(objects);
  BLB_TestContext_Shutdown(&app);
  return 0;
}
