#ifndef BLB_TESTS_H
#define BLB_TESTS_H

#include <stdbool.h>
#include <stddef.h>

typedef enum { BLB_TEST_OBJECT_CUBE = 0, BLB_TEST_OBJECT_SPHERE = 1, BLB_TEST_OBJECT_TORUS = 2, BLB_TEST_OBJECT_TEAPOT = 3 } BLB_TestObjectType;
typedef enum { BLB_TEST_OBJECT2D_SQUARE = 0, BLB_TEST_OBJECT2D_CIRCLE = 1 } BLB_TestObject2DType;

int BLB_TestMaterial(void);
int BLB_TestTexture(void);
int BLB_TestSpriteSheet(void);
int BLB_TestObjectCount(size_t count, BLB_TestObjectType type, bool r);
int BLB_TestObjects(void);
int BLB_TestSuperNova(void);
int BLB_AnimationTest(void);

int BLB_Test_ph3d(void);
int BLB_TestPhysicsObjectCount(size_t count, BLB_TestObjectType type);
int BLB_TestObjectCount2D(size_t count, BLB_TestObject2DType type, bool r);
int BLB_TestPhysicsObjectCount2D(size_t count, BLB_TestObject2DType type);

int BLB_Test_ph2d(void);
int BLB_TestParticles2D(void);
int BLB_TestParticles3D(void);
int BLB_TestHandlers(void);

int BLB_TestSound(void);

#endif
