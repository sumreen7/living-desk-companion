#pragma once

#include "app_state.h"
#include "canvas.h"
#include "sky.h"

void moodCycle();
void moodSet(Mood mood);
const char* moodName(Mood mood);
void moodDraw(Canvas& canvas, const SkyLook& look, const AppState& state, uint32_t nowMs);
