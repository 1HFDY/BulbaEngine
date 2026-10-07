#include <bulba/core/utils/constants.h>
#include <math.h>

const float BLB_PI = 3.14159265358979323846f;
const float BLB_TAU = 6.28318530717958647692f;
const float BLB_HALF_PI = 1.57079632679489661923f;
const float BLB_QUARTER_PI = 0.78539816339744830962f;

const float BLB_E = 2.71828182845904523536f;
const float BLB_SQRT2 = 1.41421356237309504880f;
const float BLB_SQRT3 = 1.73205080756887729353f;

const float BLB_DEG2RAD = 0.01745329251994329577f;
const float BLB_RAD2DEG = 57.2957795130823208768f;

const float BLB_EPSILON = 1e-6f;
const float BLB_EPSILON_SMALL = 1e-8f;
const float BLB_EPSILON_LARGE = 1e-4f;

const float BLB_INFINITY = INFINITY;
const float BLB_NEGATIVE_INFINITY = -INFINITY;

// Time
const float BLB_DEFAULT_FPS = 60.0f;
const float BLB_DEFAULT_DELTA_TIME = 1.0f / 60.0f;

// Physics
const float BLB_GRAVITY = 9.81f;
const float BLB_GRAVITY_X = 0.0f;
const float BLB_GRAVITY_Y = -9.81f;
const float BLB_GRAVITY_Z = 0.0f;

const float BLB_DEFAULT_PHYSICS_FPS = 60.0f;
const float BLB_DEFAULT_PHYSICS_DELTA_TIME = 1.0f / 60.0f;

const float BLB_DEFAULT_FRICTION = 0.5f;
const float BLB_DEFAULT_RESTITUTION = 0.0f;
const float BLB_DEFAULT_LINEAR_DAMPING = 0.1f;
const float BLB_DEFAULT_ANGULAR_DAMPING = 0.1f;

// Transform
const float BLB_DEFAULT_POSITION = 0.0f;
const float BLB_DEFAULT_ROTATION = 0.0f;
const float BLB_DEFAULT_SCALE = 1.0f;

// Camera
const float BLB_DEFAULT_FOV = 60.0f;
const float BLB_DEFAULT_NEAR_CLIP = 0.01f;
const float BLB_DEFAULT_FAR_CLIP = 10000.0f;

const float BLB_DEFAULT_CAMERA_SPEED = 5.0f;
const float BLB_DEFAULT_CAMERA_SENSITIVITY = 0.1f;

// Rendering
const float BLB_DEFAULT_EXPOSURE = 1.0f;
const float BLB_DEFAULT_GAMMA = 2.2f;

const float BLB_DEFAULT_AMBIENT_STRENGTH = 0.1f;

const float BLB_DEFAULT_SHADOW_BIAS = 0.005f;

// Material
const float BLB_DEFAULT_METALLIC = 0.0f;
const float BLB_DEFAULT_ROUGHNESS = 0.5f;
const float BLB_DEFAULT_SPECULAR = 0.5f;
const float BLB_DEFAULT_IOR = 1.5f;
const float BLB_DEFAULT_TRANSMISSION = 0.0f;

const float BLB_DEFAULT_CLEARCOAT = 0.0f;
const float BLB_DEFAULT_SHEEN = 0.0f;
const float BLB_DEFAULT_IRIDESCENCE = 0.0f;
const float BLB_DEFAULT_ANISOTROPY = 0.0f;

const float BLB_DEFAULT_EMISSION = 0.0f;
const float BLB_DEFAULT_EMISSION_STRENGTH = 1.0f;

const float BLB_DEFAULT_ALPHA = 1.0f;

// Lighting
const float BLB_DEFAULT_LIGHT_INTENSITY = 1.0f;
const float BLB_DEFAULT_LIGHT_RANGE = 10.0f;

// Audio
const float BLB_DEFAULT_AUDIO_VOLUME = 1.0f;
const float BLB_DEFAULT_AUDIO_PITCH = 1.0f;

// Particles
const float BLB_DEFAULT_PARTICLE_LIFETIME = 1.0f;
const float BLB_DEFAULT_PARTICLE_SCALE = 1.0f;

// Colors: RGBA8
const HMM_Vec4 BLB_COLOR_BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const HMM_Vec4 BLB_COLOR_WHITE = {1.0f, 1.0f, 1.0f, 1.0f};

const HMM_Vec4 BLB_COLOR_RED = {1.0f, 0.0f, 0.0f, 1.0f};
const HMM_Vec4 BLB_COLOR_GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const HMM_Vec4 BLB_COLOR_BLUE = {0.0f, 0.0f, 1.0f, 1.0f};

const HMM_Vec4 BLB_COLOR_YELLOW = {1.0f, 1.0f, 0.0f, 1.0f};
const HMM_Vec4 BLB_COLOR_CYAN = {0.0f, 1.0f, 1.0f, 1.0f};
const HMM_Vec4 BLB_COLOR_MAGENTA = {1.0f, 0.0f, 1.0f, 1.0f};

const HMM_Vec4 BLB_COLOR_GRAY = {0.5f, 0.5f, 0.5f, 1.0f};

const HMM_Vec4 BLB_COLOR_TRANSPARENT = {0.0f, 0.0f, 0.0f, 0.0f};

// Common values
const float BLB_ZERO = 0.0f;
const float BLB_ONE = 1.0f;
const float BLB_HALF = 0.5f;
