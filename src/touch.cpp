#include "touch.h"

#include <Arduino.h>

#include "config.h"
#include "display.h"
#include "log.h"

static bool gDown = false;
static int gStartX = 0;
static int gStartY = 0;
static int gLastX = 0;
static int gLastY = 0;
static uint32_t gStartMs = 0;
static uint32_t gLastSeenMs = 0;
static uint32_t gMuteUntil = 0;
static uint8_t gRecent = 0;
static uint32_t gRecentStart = 0;

static Gesture classify(uint32_t nowMs) {
    Gesture gesture{GESTURE_NONE, (int16_t)gLastX, (int16_t)gLastY};
    int dx = gLastX - gStartX;
    int dy = gLastY - gStartY;
    uint32_t held = nowMs - gStartMs;
    if (held < 40) {
        return gesture;
    }
    if (abs(dx) >= 36 && abs(dx) > abs(dy) + 8 && held < 700) {
        gesture.kind = dx < 0 ? GESTURE_SWIPE_LEFT : GESTURE_SWIPE_RIGHT;
    } else if (abs(dx) < 16 && abs(dy) < 16 && held < 500) {
        gesture.kind = GESTURE_TAP;
    }
    return gesture;
}

void touchBegin() {
    gDown = false;
}

Gesture touchPoll(uint32_t nowMs) {
    Gesture none{GESTURE_NONE, 0, 0};
    if (!displayPanelReady()) {
        return none;
    }
    if ((int32_t)(nowMs - gMuteUntil) < 0) {
        return none;
    }

    int32_t x = 0;
    int32_t y = 0;
    bool pressed = displayDevice().getTouch(&x, &y);
    if (pressed && (x < 0 || y < 0 || x >= LPC_SCREEN_WIDTH || y >= LPC_SCREEN_HEIGHT)) {
        pressed = false;
    }

    if (pressed) {
        if (!gDown) {
            gDown = true;
            gStartX = gLastX = x;
            gStartY = gLastY = y;
            gStartMs = nowMs;
        } else {
            gLastX = x;
            gLastY = y;
        }
        gLastSeenMs = nowMs;
        return none;
    }

    if (!gDown || (nowMs - gLastSeenMs) < 70) {
        return none;
    }

    gDown = false;
    Gesture gesture = classify(nowMs);
    if (gesture.kind == GESTURE_NONE) {
        return gesture;
    }

    if (gRecentStart == 0 || nowMs - gRecentStart > 2000) {
        gRecentStart = nowMs;
        gRecent = 0;
    }
    gRecent++;
    if (gRecent > 6) {
        gMuteUntil = nowMs + 5000;
        gRecent = 0;
        logLine("touch: paused for 5s because the sensor was noisy");
        return none;
    }
    return gesture;
}
