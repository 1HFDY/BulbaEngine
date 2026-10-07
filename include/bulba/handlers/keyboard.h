#ifndef BLB_HANDLERS_KEYBOARD_H
#define BLB_HANDLERS_KEYBOARD_H

#include "bulba/handlers/handlers.h"

#include <stdbool.h>

struct GLFWwindow;

int BLB_KeyboardAttach(BLB_Handlers *handlers, struct GLFWwindow *window);
void BLB_KeyboardDetach(BLB_Handlers *handlers);

const BLB_KeyState *BLB_KeyboardGet(const BLB_Handlers *handlers, BLB_Key key);
BLB_KeyEvent BLB_KeyboardInfo(const BLB_Handlers *handlers, BLB_Key key);

bool BLB_KeyboardDown(const BLB_Handlers *handlers, BLB_Key key);
bool BLB_KeyboardHeld(const BLB_Handlers *handlers, BLB_Key key);
bool BLB_KeyboardPressed(const BLB_Handlers *handlers, BLB_Key key);
bool BLB_KeyboardReleased(const BLB_Handlers *handlers, BLB_Key key);
bool BLB_KeyboardRepeated(const BLB_Handlers *handlers, BLB_Key key);

bool BLB_KeyboardSetHandler(BLB_Handlers *handlers, BLB_HandlerCallback callback, void *userdata);

void BLB_KeyboardClearTransitions(BLB_Handlers *handlers);

#endif
