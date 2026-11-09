#pragma once

#include <stdint.h>

#include "app_state.h"
#include "canvas.h"

struct SkyLook {
    uint16_t top;
    uint16_t bottom;
    uint16_t sun;
    uint16_t moon;
    uint16_t cloud;
    uint16_t hill;
    uint16_t plate;
    uint16_t ink;
    uint16_t clock;
    bool stars;
    bool showSun;
    bool showMoon;
    SkyPeriod period;
};

SkyLook skyLookFor(const AppState& state, int minuteOfDay);
void skyDraw(Canvas& canvas, const SkyLook& look, const AppState& state, int minuteOfDay, uint32_t nowMs, int skyHeight);
