#ifndef BLB_OBJECT_OPTIMIZATION2D_H
#define BLB_OBJECT_OPTIMIZATION2D_H

#include "bulba/core/objects2d/objects2d.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct BLB_RenderInstances2DGroup {
  BLB_Object2D **objects;
  size_t object_count;
  size_t object_capacity;
  const BLB_Polygon2D *polygon;
  BLB_Material *material;
  BLB_Texture *texture;
  const BLB_ShaderProgram *shader;
  unsigned short layer;
  BLB_RenderMode render_mode;
  unsigned char color[4];
  bool screen_space;
  bool enabled;
  bool rendered;
} BLB_RenderInstances2DGroup;

typedef struct BLB_ObjectOptimization2D {
  BLB_Object2D **source_objects;
  size_t source_count;
  uint64_t source_signature;
  BLB_RenderInstances2DGroup **groups;
  size_t group_count;
  size_t group_map_capacity;
  const BLB_Object2D **group_map_keys;
  BLB_RenderInstances2DGroup **group_map_values;
} BLB_ObjectOptimization2D;

BLB_ObjectOptimization2D *BLB_ObjectOptimization2D_Create(BLB_Object2D **objects, size_t count);
void BLB_ObjectOptimization2D_Destroy(BLB_ObjectOptimization2D *optimization);
bool BLB_ObjectOptimization2D_MatchesSource(const BLB_ObjectOptimization2D *optimization, BLB_Object2D **objects, size_t count);
BLB_RenderInstances2DGroup *BLB_ObjectOptimization2D_FindGroup(const BLB_ObjectOptimization2D *optimization, const BLB_Object2D *object);
void BLB_ObjectOptimization2D_ResetDrawState(BLB_ObjectOptimization2D *optimization);

#endif
