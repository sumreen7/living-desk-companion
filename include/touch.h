#pragma once

#include <stdint.h>

enum GestureKind : uint8_t {
    GESTURE_NONE = 0,
    GESTURE_TAP,
    GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT
};

struct Gesture {
    GestureKind kind;
    int16_t x;
    int16_t y;
};

void touchBegin();
Gesture touchPoll(uint32_t nowMs);
