#include "navigation.h"

#include <Arduino.h>

#include "app_state.h"
#include "log.h"

static int gShift = 0;
static int gSign = 0;
static uint32_t gStartMs = 0;
static bool gAnimating = false;

static const char* pageName(Page page) {
    switch (page) {
        case PAGE_SKY: return "sky";
        case PAGE_WEATHER: return "weather";
        case PAGE_MUSIC: return "music";
        case PAGE_MOOD: return "mood";
        default: return "page";
    }
}

void navigationNudge(int direction) {
    if (direction == 0) {
        return;
    }
    int sign = direction > 0 ? 1 : -1;
    {
        StateGuard guard;
        int next = (int)guard.get().page + sign;
        if (next < 0) {
            next = PAGE_COUNT - 1;
        }
        if (next >= PAGE_COUNT) {
            next = 0;
        }
        guard.get().page = (Page)next;
        logf("page: %s", pageName(guard.get().page));
    }
    gSign = sign;
    gShift = -sign * 240;
    gStartMs = millis();
    gAnimating = true;
}

void navigationTick(uint32_t nowMs) {
    if (!gAnimating) {
        return;
    }
    uint32_t elapsed = nowMs - gStartMs;
    if (elapsed >= 220) {
        gShift = 0;
        gAnimating = false;
        return;
    }
    int t = (int)(elapsed * 255 / 220);
    int eased = t * t * (3 * 255 - 2 * t) / (255 * 255);
    gShift = -gSign * 240 * (255 - eased) / 255;
}

int navigationShift() {
    return gShift;
}
