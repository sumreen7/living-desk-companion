#pragma once

#include "app_state.h"
#include "canvas.h"
#include "sky.h"

void musicToggle();
void musicDraw(Canvas& canvas, const SkyLook& look, const AppState& state, uint32_t nowMs);
