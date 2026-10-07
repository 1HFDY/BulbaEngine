#ifndef BLB_OBJECT_LOD_H
#define BLB_OBJECT_LOD_H

#include "bulba/core/objects2d/objects2d.h"
#include "bulba/core/objects3d/objects3d.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Per-object LOD overrides are optional; without an override the global config and distance policy are used. */
bool BLB_Object3D_SetLODLevel(BLB_Object3D *object, int level);
bool BLB_Object2D_SetLODLevel(BLB_Object2D *object, int level);

/* Removes the per-object override so global automatic LOD settings apply. */
void BLB_Object3D_ResetLODOverride(BLB_Object3D *object);
void BLB_Object2D_ResetLODOverride(BLB_Object2D *object);

int BLB_Object3D_GetLODLevel(const BLB_Object3D *object);
int BLB_Object3D_GetAvailableLODLevel(const BLB_Object3D *object, int requested_level);
int BLB_Object2D_GetLODLevel(const BLB_Object2D *object);
int BLB_Object2D_GetAvailableLODLevel(const BLB_Object2D *object, int requested_level);

const BLB_Polygon3D *BLB_Object3D_GetLODPolygon(const BLB_Object3D *object);
const BLB_Polygon2D *BLB_Object2D_GetLODPolygon(const BLB_Object2D *object);

/* Returns a shared or object-owned polygon for the requested render LOD. */
const BLB_Polygon3D *BLB_Object3D_GetLODPolygonAtLevel(const BLB_Object3D *object, int level);
const BLB_Polygon2D *BLB_Object2D_GetLODPolygonAtLevel(const BLB_Object2D *object, int level);

/* Generates and attaches all levels needed by explicit per-object LOD. */
bool BLB_Object3D_EnsureLODLevel(BLB_Object3D *object, int level);
bool BLB_Object2D_EnsureLODLevel(BLB_Object2D *object, int level);

/* Resolves the final render LOD from the object override, global base level, and optional distance reduction. */
int BLB_Object3D_ResolveLODLevel(const BLB_Object3D *object, float distance);
int BLB_Object2D_ResolveLODLevel(const BLB_Object2D *object, float distance);

void BLB_Object3D_ClearLODs(BLB_Object3D *object);
void BLB_Object2D_ClearLODs(BLB_Object2D *object);

void BLB_ObjectLOD_BeginFrame(uint64_t frame_index);
void BLB_ObjectLODClearCache(void);
void BLB_ObjectLOD_GetCacheStats(size_t *entries, uint64_t *hits, uint64_t *misses, size_t *live_refs);

#endif
