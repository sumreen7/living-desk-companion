#pragma once

#include "touch.h"

void uiRender(uint32_t nowMs);
void uiHandleGesture(const Gesture& gesture);
void uiHandleKey(char key);
