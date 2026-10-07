#ifndef BLB_HANDLERS_MOUSE_H
#define BLB_HANDLERS_MOUSE_H

#include "bulba/handlers/handlers.h"

#include <stdbool.h>

struct GLFWwindow;

int BLB_MouseAttach(BLB_Handlers *handlers, struct GLFWwindow *window);
void BLB_MouseDetach(BLB_Handlers *handlers);

typedef struct {
  double x;
  double y;
  double delta_x;
  double delta_y;
  double scroll_x;
  double scroll_y;
  bool inside;
  bool moved;
  uint16_t down_mask;
  uint16_t pressed_mask;
  uint16_t released_mask;
} BLB_MouseInfo;

const BLB_MouseButtonState *BLB_MouseButtonGet(const BLB_Handlers *handlers, int button);
BLB_MouseButtonEvent BLB_MouseButtonInfo(const BLB_Handlers *handlers, int button);
BLB_MouseInfo BLB_MouseGet(const BLB_Handlers *handlers);
bool BLB_MouseDown(const BLB_Handlers *handlers, int button);
bool BLB_MouseHeld(const BLB_Handlers *handlers, int button);
bool BLB_MousePressed(const BLB_Handlers *handlers, int button);
bool BLB_MouseReleased(const BLB_Handlers *handlers, int button);
bool BLB_MouseSetButtonHandler(BLB_Handlers *handlers, BLB_HandlerCallback callback, void *userdata);
bool BLB_MouseSetMoveHandler(BLB_Handlers *handlers, BLB_HandlerCallback callback, void *userdata);
bool BLB_MouseSetScrollHandler(BLB_Handlers *handlers, BLB_HandlerCallback callback, void *userdata);

#endif
