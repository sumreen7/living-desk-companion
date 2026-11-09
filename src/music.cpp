#include "music.h"

#include <math.h>

#include "color.h"
#include "config.h"

static float gAngle = 18.0f;
static uint32_t gLastMs = 0;
static bool gClockStarted = false;

void musicToggle() {
    StateGuard guard;
    guard.get().musicPlaying = !guard.get().musicPlaying;
}

static void spin(uint32_t nowMs, bool playing) {
    if (!gClockStarted) {
        gLastMs = nowMs;
        gClockStarted = true;
    }
    if (playing) {
        gAngle += (nowMs - gLastMs) * 0.12f;
        if (gAngle >= 360.0f) {
            gAngle -= 360.0f;
        }
    }
    gLastMs = nowMs;
}

void musicDraw(Canvas& canvas, const SkyLook& look, const AppState& state, uint32_t nowMs) {
    skyDraw(canvas, look, state, -1, nowMs, 96);
    canvas.fillRect(0, 88, LPC_SCREEN_WIDTH, 200, look.plate);

    spin(nowMs, state.musicPlaying);
    const int cx = 120;
    const int cy = 176;
    canvas.fillEllipse(cx + 3, cy + 8, 66, 16, dim565(look.hill, 2, 3));
    canvas.fillCircle(cx, cy, 64, hex565(0x1C1A24));
    for (int r = 28; r <= 58; r += 7) {
        canvas.drawCircle(cx, cy, r, hex565(0x2E2A38));
    }

    float rad = gAngle * 0.0174533f;
    int sx = cx + (int)(cosf(rad) * 46);
    int sy = cy + (int)(sinf(rad) * 46);
    canvas.fillCircle(sx, sy, 4, hex565(0xF7F1E6));

    canvas.fillCircle(cx, cy, 24, hex565(0xF3B89A));
    static const uint32_t kDots[] = {0xCDB4DB, 0x243044, 0xFFF6E4, 0xE07A9A};
    for (int i = 0; i < 4; i++) {
        float a = (gAngle + i * 90.0f) * 0.0174533f;
        canvas.fillCircle(cx + (int)(cosf(a) * 12), cy + (int)(sinf(a) * 12), 4, hex565(kDots[i]));
    }
    canvas.fillCircle(cx, cy, 3, hex565(0xF6F1E8));

    canvas.setFont(&fonts::Font2);
    canvas.setTextSize(1);
    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setTextColor(look.ink);
    if (!state.musicPlaying) {
        canvas.drawString("paused", cx, 268);
    }
}
