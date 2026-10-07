#include "bulba/handlers/handlers.h"
#include "bulba/handlers/object.h"
#include "test_common.h"
#include "tests.h"

#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define LOG_LINES 17
#define TEST_WIDTH 1200.0f
#define TEST_HEIGHT 900.0f
#define PANEL_WIDTH 430.0f
#define PANEL_HEIGHT 760.0f
#define PANEL_LEFT 18.0f
#define PANEL_TOP (TEST_HEIGHT - 24.0f)
#define PANEL_ANIMATION_SPEED 14.0f
#define PANEL_EPSILON 0.001f

typedef struct {
  BLB_TestContext *app;
  BLB_Handlers handlers;
  BLB_Object2D *object2d;
  BLB_Object3D *object3d;
  BLB_Object2D *panel;
  BLB_Object2D *panel_accent;
  BLB_Text2D *panel_title;
  BLB_Text2D *texts[LOG_LINES];
  char text_buffers[LOG_LINES][192];
  BLB_ObjectHandler handler2d;
  BLB_ObjectHandler handler3d;
  BLB_ObjectHandlerObjectType active_type;
  bool dragging;
  bool dragging_3d;
  bool f3_was_down;
  bool panel_open;
  bool has_last_event;
  float panel_progress;
  HMM_Vec3 object3d_world_offset;
  HMM_Vec3 drag_plane_origin;
  HMM_Vec3 drag_plane_normal;
  BLB_ObjectEvent last_event;
  int clicks_2d;
  int clicks_3d;
} HandlerTestState;

static void setup_text_material(BLB_Text2D *text) {
  if (!text || !text->material)
    return;

  BLB_Material_SetLighting(text->material, false);
  BLB_Material_SetUnlit(text->material, true);
  BLB_Material_SetDepth(text->material, false, false);
  BLB_Material_SetDoubleSided(text->material, true);
}

static BLB_Text2D *make_text(const char *value, HMM_Vec2 position, float size, unsigned short layer) {
  BLB_Text2D *text = BLB_CreateText2D(value, NULL, position, size, true);

  if (!text)
    return NULL;

  text->layer = layer;
  text->color[0] = 220;
  text->color[1] = 225;
  text->color[2] = 235;
  text->color[3] = 255;

  setup_text_material(text);

  return text;
}

static BLB_Object2D *make_panel(HMM_Vec2 size, HMM_Vec2 position, unsigned char r, unsigned char g, unsigned char b, unsigned short layer) {
  BLB_Object2D *object = BLB_CreateSquare2D(size, position, NULL, true);

  if (!object)
    return NULL;

  object->layer = layer;
  object->color.Elements[0] = r;
  object->color.Elements[1] = g;
  object->color.Elements[2] = b;
  object->color.Elements[3] = 255;

  if (object->material) {
    BLB_Material_SetBaseColor(object->material, r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
    BLB_Material_SetLighting(object->material, false);
    BLB_Material_SetUnlit(object->material, true);
    BLB_Material_SetDepth(object->material, false, false);
    BLB_Material_SetDoubleSided(object->material, true);
    BLB_Material_SetRenderMode(object->material, BLB_RENDER_OPAQUE);
  }

  return object;
}

static const char *event_name(BLB_ObjectHandlerEventType type) {
  switch (type) {
  case BLB_OBJECT_HANDLER_CLICK:
    return "CLICK";
  case BLB_OBJECT_HANDLER_HOLD:
    return "HOLD";
  default:
    return "NONE";
  }
}

static const char *type_name(BLB_ObjectHandlerObjectType type) {
  switch (type) {
  case BLB_OBJECT_HANDLER_2D:
    return "2D";
  case BLB_OBJECT_HANDLER_3D:
    return "3D";
  default:
    return "NONE";
  }
}

static void set_line(HandlerTestState *state, size_t index, const char *value) {
  if (!state || index >= LOG_LINES || !state->texts[index])
    return;

  snprintf(state->text_buffers[index], sizeof(state->text_buffers[index]), "%s", value ? value : "");
  BLB_SetText2D(state->texts[index], state->text_buffers[index]);
}

static void set_info(HandlerTestState *state, const BLB_ObjectEvent *event) {
  if (!state || !event)
    return;

  state->last_event = *event;
  state->has_last_event = true;

  char buffer[192];

  snprintf(buffer, sizeof(buffer), "EVENT       %-6s   TARGET %s", event_name(event->type), type_name(event->object_type));
  set_line(state, 0, buffer);

  snprintf(buffer, sizeof(buffer), "BUTTON      %-2d      MODS %u", event->button, event->mods);
  set_line(state, 1, buffer);

  snprintf(buffer, sizeof(buffer), "FRAME       %-10llu TIME %.4f", (unsigned long long)event->frame, event->time);
  set_line(state, 2, buffer);

  snprintf(buffer, sizeof(buffer), "MOUSE       %8.1f %8.1f", event->mouse_x, event->mouse_y);
  set_line(state, 3, buffer);

  snprintf(buffer, sizeof(buffer), "DELTA       %8.1f %8.1f", event->mouse_dx, event->mouse_dy);
  set_line(state, 4, buffer);

  snprintf(buffer, sizeof(buffer), "WORLD       %8.3f %8.3f", event->world_position.x, event->world_position.y);
  set_line(state, 5, buffer);

  snprintf(buffer, sizeof(buffer), "RAY ORIGIN  %8.3f %8.3f %8.3f", event->ray_origin.x, event->ray_origin.y, event->ray_origin.z);
  set_line(state, 6, buffer);

  snprintf(buffer, sizeof(buffer), "RAY DIR     %8.3f %8.3f %8.3f", event->ray_direction.x, event->ray_direction.y, event->ray_direction.z);
  set_line(state, 7, buffer);

  snprintf(buffer, sizeof(buffer), "HIT POINT   %8.3f %8.3f %8.3f", event->hit_point.x, event->hit_point.y, event->hit_point.z);
  set_line(state, 8, buffer);

  snprintf(buffer, sizeof(buffer), "HIT NORMAL  %8.3f %8.3f %8.3f", event->hit_normal.x, event->hit_normal.y, event->hit_normal.z);
  set_line(state, 9, buffer);

  snprintf(buffer, sizeof(buffer), "DISTANCE    %.5f      T %.5f", event->distance, event->t);
  set_line(state, 10, buffer);

  snprintf(buffer, sizeof(buffer), "HOVERED     %-3s       CAPTURED %-3s", event->hovered ? "YES" : "NO", event->captured ? "YES" : "NO");
  set_line(state, 11, buffer);

  snprintf(buffer, sizeof(buffer), "INSIDE      %-3s       DRAGGING %-3s", event->inside ? "YES" : "NO", state->dragging ? "YES" : "NO");
  set_line(state, 12, buffer);

  snprintf(buffer, sizeof(buffer), "CLICKS      2D %-6d  3D %-6d", state->clicks_2d, state->clicks_3d);
  set_line(state, 13, buffer);

  snprintf(buffer, sizeof(buffer), "2D POS      %8.2f %8.2f", state->object2d ? state->object2d->position.x : 0.0f,
           state->object2d ? state->object2d->position.y : 0.0f);
  set_line(state, 14, buffer);

  snprintf(buffer, sizeof(buffer), "3D POS      %8.2f %8.2f %8.2f", state->object3d ? state->object3d->position.x : 0.0f,
           state->object3d ? state->object3d->position.y : 0.0f, state->object3d ? state->object3d->position.z : 0.0f);
  set_line(state, 15, buffer);

  snprintf(buffer, sizeof(buffer), "STATUS      %-10s  F3 TOGGLE", state->active_type ? type_name(state->active_type) : "IDLE");
  set_line(state, 16, buffer);
}

static void refresh_info(HandlerTestState *state, BLB_Scene *scene) {
  if (!state || !scene || !state->has_last_event)
    return;

  BLB_ObjectEvent event = state->last_event;

  event.frame = scene->frame_index;
  event.time = glfwGetTime();
  event.mouse_x = state->handlers.mouse_x;
  event.mouse_y = state->handlers.mouse_y;
  event.mouse_dx = state->handlers.mouse_dx;
  event.mouse_dy = state->handlers.mouse_dy;
  event.captured = state->dragging;
  event.hovered = state->handler2d.hovered || state->handler3d.hovered;

  if (state->object2d)
    event.object.object2d = state->object2d;

  if (state->object3d)
    event.object.object3d = state->object3d;

  set_info(state, &event);
}

static void set_object_color(BLB_Object2D *object2d, BLB_Object3D *object3d, BLB_ObjectHandlerObjectType type, bool hot, bool active) {
  if (type == BLB_OBJECT_HANDLER_2D && object2d) {
    object2d->color.Elements[0] = active ? 255 : hot ? 90 : 60;
    object2d->color.Elements[1] = active ? 255 : hot ? 220 : 150;
    object2d->color.Elements[2] = 255;
    object2d->color.Elements[3] = 255;
  }

  if (type == BLB_OBJECT_HANDLER_3D && object3d) {
    object3d->color.Elements[0] = 255;
    object3d->color.Elements[1] = active ? 255 : hot ? 215 : 145;
    object3d->color.Elements[2] = active ? 100 : 45;
    object3d->color.Elements[3] = 255;
  }
}

static HMM_Vec3 camera_forward(const BLB_Camera *camera) {
  if (!camera)
    return HMM_V3(0.0f, 0.0f, -1.0f);

  const float yaw = HMM_AngleDeg(camera->rotation.y);
  const float pitch = HMM_AngleDeg(camera->rotation.x);
  const float cos_pitch = cosf(pitch);

  return HMM_NormV3(HMM_V3(cos_pitch * sinf(yaw), -sinf(pitch), -cos_pitch * cosf(yaw)));
}

static bool ray_plane(HMM_Vec3 ray_origin, HMM_Vec3 ray_direction, HMM_Vec3 plane_origin, HMM_Vec3 plane_normal, HMM_Vec3 *out_point) {
  if (!out_point)
    return false;

  const float denominator = HMM_DotV3(ray_direction, plane_normal);

  if (fabsf(denominator) < 1e-7f)
    return false;

  const float t = HMM_DotV3(HMM_SubV3(plane_origin, ray_origin), plane_normal) / denominator;

  if (t < 0.0f)
    return false;

  *out_point = HMM_AddV3(ray_origin, HMM_MulV3F(ray_direction, t));

  return true;
}

static void on_click(const BLB_ObjectEvent *event, void *userdata) {
  HandlerTestState *state = userdata;

  if (!state || !event || event->button != GLFW_MOUSE_BUTTON_LEFT)
    return;

  state->dragging = false;
  state->dragging_3d = false;
  state->active_type = event->object_type;

  if (event->object_type == BLB_OBJECT_HANDLER_2D) {
    ++state->clicks_2d;
    set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_2D, true, true);
  } else if (event->object_type == BLB_OBJECT_HANDLER_3D) {
    ++state->clicks_3d;
    set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_3D, true, true);
  }

  set_info(state, event);
}

static void on_hold(const BLB_ObjectEvent *event, void *userdata) {
  HandlerTestState *state = userdata;

  if (!state || !event || event->button != GLFW_MOUSE_BUTTON_LEFT)
    return;

  state->dragging = true;
  state->active_type = event->object_type;

  if (event->object_type == BLB_OBJECT_HANDLER_2D && state->object2d) {
    state->object2d->position.x += (float)event->mouse_dx;
    state->object2d->position.y -= (float)event->mouse_dy;
    set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_2D, true, true);
  } else if (event->object_type == BLB_OBJECT_HANDLER_3D && state->object3d && state->app && state->app->camera) {
    HMM_Vec3 world_position;

    if (!state->dragging_3d) {
      state->drag_plane_origin = state->object3d->position;
      state->drag_plane_normal = camera_forward(state->app->camera);

      if (!ray_plane(event->ray_origin, event->ray_direction, state->drag_plane_origin, state->drag_plane_normal, &world_position))
        return;

      state->object3d_world_offset = HMM_SubV3(state->object3d->position, world_position);
      state->dragging_3d = true;
    } else {
      if (!ray_plane(event->ray_origin, event->ray_direction, state->drag_plane_origin, state->drag_plane_normal, &world_position))
        return;
    }

    state->object3d->position = HMM_AddV3(world_position, state->object3d_world_offset);
    set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_3D, true, true);
  }

  set_info(state, event);
}

static BLB_ObjectHandler make_handler(HandlerTestState *state) {
  BLB_ObjectHandler handler;

  BLB_ObjectHandlerInit(&handler);

  handler.on_click = on_click;
  handler.on_hold = on_hold;
  handler.userdata = state;

  return handler;
}

static void update_hover_colors(HandlerTestState *state) {
  if (!state || state->dragging)
    return;

  set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_2D, state->handler2d.hovered, state->active_type == BLB_OBJECT_HANDLER_2D);
  set_object_color(state->object2d, state->object3d, BLB_OBJECT_HANDLER_3D, state->handler3d.hovered, state->active_type == BLB_OBJECT_HANDLER_3D);
}

static void set_panel_visibility(HandlerTestState *state, bool visible) {
  if (!state)
    return;

  if (state->panel)
    state->panel->visible = visible;

  if (state->panel_accent)
    state->panel_accent->visible = visible;

  if (state->panel_title)
    state->panel_title->visible = visible;

  for (size_t i = 0; i < LOG_LINES; ++i) {
    if (state->texts[i])
      state->texts[i]->visible = visible;
  }
}

static void update_panel_positions(HandlerTestState *state, float progress) {
  if (!state)
    return;

  const float hidden_left = -PANEL_WIDTH - 32.0f;
  const float left = hidden_left + (PANEL_LEFT - hidden_left) * progress;
  const float top = PANEL_TOP;

  if (state->panel)
    state->panel->position = HMM_V2(left + PANEL_WIDTH * 0.5f, top - PANEL_HEIGHT * 0.5f);

  if (state->panel_accent)
    state->panel_accent->position = HMM_V2(left + 4.0f, top - PANEL_HEIGHT * 0.5f);

  if (state->panel_title)
    state->panel_title->position = HMM_V2(left + 28.0f, top - 18.0f);

  for (size_t i = 0; i < LOG_LINES; ++i) {
    if (!state->texts[i])
      continue;

    state->texts[i]->position = HMM_V2(left + 24.0f, top - 58.0f - (float)i * 38.0f);
  }
}

static void update_panel(HandlerTestState *state, float delta_time) {
  if (!state)
    return;

  const float target = state->panel_open ? 1.0f : 0.0f;
  const float dt = fmaxf(delta_time, 0.0f);
  const float factor = 1.0f - expf(-PANEL_ANIMATION_SPEED * dt);

  if (state->panel_open)
    set_panel_visibility(state, true);

  state->panel_progress += (target - state->panel_progress) * factor;

  if (fabsf(target - state->panel_progress) <= PANEL_EPSILON)
    state->panel_progress = target;

  update_panel_positions(state, state->panel_progress);

  if (!state->panel_open && state->panel_progress <= PANEL_EPSILON)
    set_panel_visibility(state, false);
}

int BLB_TestHandlers(void) {
  BLB_TestContext app;

  if (BLB_TestContext_Init(&app, 1200, 900, "BulbaEngine - Object Handlers", HMM_V3(0.0f, 0.0f, 12.0f), 9, 11, 16) != 0)
    return -1;

  BLB_TestContext_AddLight(&app, BLB_LIGHT_POINT, HMM_V3(-4.0f, 5.0f, 8.0f), HMM_V3(0.0f, 0.0f, 0.0f), 18.0f, 0.03f, 1.0f, 30.0f);
  BLB_TestContext_AddLight(&app, BLB_LIGHT_POINT, HMM_V3(5.0f, 4.0f, 6.0f), HMM_V3(0.0f, 0.0f, 0.0f), 7.0f, 0.01f, 0.6f, 24.0f);

  HandlerTestState state;
  memset(&state, 0, sizeof(state));

  state.app = &app;
  state.panel_progress = 0.0f;
  state.panel_open = false;

  state.object2d = BLB_CreateSquare2D(HMM_V2(190.0f, 150.0f), HMM_V2(890.0f, 360.0f), NULL, true);
  state.object3d = BLB_CreateCube3D(HMM_V3(2.6f, 2.6f, 2.6f), HMM_V3(-2.8f, 0.0f, 0.0f), NULL, BLB_TEXMAP_STRETCH);

  state.panel =
      make_panel(HMM_V2(PANEL_WIDTH, PANEL_HEIGHT), HMM_V2(PANEL_LEFT + PANEL_WIDTH * 0.5f, PANEL_TOP - PANEL_HEIGHT * 0.5f), 18, 22, 31, 1000);
  state.panel_accent = make_panel(HMM_V2(5.0f, PANEL_HEIGHT), HMM_V2(PANEL_LEFT + 4.0f, PANEL_TOP - PANEL_HEIGHT * 0.5f), 80, 210, 255, 1001);
  state.panel_title = make_text("EVENT INSPECTOR", HMM_V2(PANEL_LEFT + 28.0f, PANEL_TOP - 18.0f), 21.0f, 1100);

  if (!state.object2d || !state.object3d || !state.panel) {
    BLB_DestroySquare2D(state.object2d);
    BLB_DestroyCube3D(state.object3d);
    BLB_DestroySquare2D(state.panel);
    BLB_DestroySquare2D(state.panel_accent);
    BLB_DestroyText2D(state.panel_title);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  state.object2d->layer = 10;
  state.object2d->color.Elements[0] = 60;
  state.object2d->color.Elements[1] = 150;
  state.object2d->color.Elements[2] = 255;
  state.object2d->color.Elements[3] = 255;

  state.object3d->layer = 10;
  state.object3d->color.Elements[0] = 255;
  state.object3d->color.Elements[1] = 145;
  state.object3d->color.Elements[2] = 45;
  state.object3d->color.Elements[3] = 255;

  set_panel_visibility(&state, false);
  update_panel_positions(&state, 0.0f);

  BLB_AddObject2D(app.scene, state.object2d);
  BLB_AddObject3D(app.scene, state.object3d);
  BLB_AddObject2D(app.scene, state.panel);

  if (state.panel_accent)
    BLB_AddObject2D(app.scene, state.panel_accent);

  if (state.panel_title)
    BLB_AddText2D(app.scene, state.panel_title);

  state.handler2d = make_handler(&state);
  state.handler3d = make_handler(&state);

  BLB_Object2D_SetHandler(state.object2d, &state.handler2d);
  BLB_Object3D_SetHandler(state.object3d, &state.handler3d);

  if (BLB_HandlersInit(&state.handlers, app.window->handle) != 0) {
    BLB_DestroySquare2D(state.panel);
    BLB_DestroySquare2D(state.panel_accent);
    BLB_DestroyText2D(state.panel_title);
    BLB_DestroySquare2D(state.object2d);
    BLB_DestroyCube3D(state.object3d);
    BLB_TestContext_Shutdown(&app);
    return -1;
  }

  for (size_t i = 0; i < LOG_LINES; ++i) {
    state.texts[i] = make_text("...", HMM_V2(42.0f, 0.0f), 13.0f, 1101);

    if (!state.texts[i])
      continue;

    state.texts[i]->visible = false;
    BLB_AddText2D(app.scene, state.texts[i]);
  }

  state.active_type = BLB_OBJECT_HANDLER_3D;

  BLB_ObjectEvent initial = BLB_ObjectEventZero();

  initial.type = BLB_OBJECT_HANDLER_CLICK;
  initial.object_type = BLB_OBJECT_HANDLER_3D;
  initial.object.object3d = state.object3d;
  initial.frame = app.scene ? app.scene->frame_index : 0;
  initial.time = glfwGetTime();
  initial.world_position = HMM_V2(state.object3d->position.x, state.object3d->position.y);
  initial.ray_origin = app.camera ? app.camera->position : HMM_V3(0.0f, 0.0f, 0.0f);
  initial.ray_direction = camera_forward(app.camera);
  initial.hit_point = state.object3d->position;
  initial.hit_normal = HMM_V3(0.0f, 0.0f, 1.0f);
  initial.distance = app.camera ? HMM_LenV3(HMM_SubV3(state.object3d->position, app.camera->position)) : 0.0f;
  initial.t = initial.distance;
  initial.hovered = false;
  initial.captured = false;
  initial.inside = false;

  set_info(&state, &initial);

  while (!BLB_WindowShouldClose(app.window)) {
    BLB_HandlersBeginFrame(&state.handlers);

    float delta_time = 0.0f;
    const int frame = BLB_TestContext_BeginFrame(&app, &delta_time);

    if (frame < 0)
      break;

    if (frame > 0)
      continue;

    BLB_HandlersProcessObjects(&state.handlers, app.scene);

    GLFWwindow *window = app.window ? app.window->handle : NULL;

    if (window) {
      const bool f3_down = glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS;

      if (f3_down && !state.f3_was_down)
        state.panel_open = !state.panel_open;

      state.f3_was_down = f3_down;
    }

    if (!state.handlers.mouse_buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
      state.dragging = false;
      state.dragging_3d = false;
    }

    refresh_info(&state, app.scene);
    update_hover_colors(&state);
    update_panel(&state, delta_time);

    if (BLB_TestContext_Draw(&app) < 0)
      break;
  }

  BLB_HandlersShutdown(&state.handlers);

  for (size_t i = 0; i < LOG_LINES; ++i)
    BLB_DestroyText2D(state.texts[i]);

  BLB_DestroyText2D(state.panel_title);
  BLB_DestroySquare2D(state.panel_accent);
  BLB_DestroySquare2D(state.panel);
  BLB_DestroySquare2D(state.object2d);
  BLB_DestroyCube3D(state.object3d);

  BLB_TestContext_Shutdown(&app);

  return 0;
}
