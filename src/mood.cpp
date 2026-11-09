#include "mood.h"

#include "color.h"
#include "config.h"

const char* moodName(Mood mood) {
    switch (mood) {
        case MOOD_STUDYING: return "Studying";
        case MOOD_WORKING: return "Working";
        case MOOD_RELAXING: return "Relaxing";
        case MOOD_LISTENING: return "Listening";
        case MOOD_BREAK: return "Taking a break";
        case MOOD_SLEEPING: return "Sleeping";
        default: return "Relaxing";
    }
}

void moodCycle() {
    StateGuard guard;
    uint8_t next = (uint8_t)((guard.get().mood + 1) % MOOD_COUNT);
    guard.get().mood = (Mood)next;
}

void moodSet(Mood mood) {
    if (mood >= MOOD_COUNT) {
        return;
    }
    StateGuard guard;
    guard.get().mood = mood;
}

static uint16_t clothFor(Mood mood) {
    switch (mood) {
        case MOOD_STUDYING: return hex565(0x3D5A80);
        case MOOD_WORKING: return hex565(0x2A6F6F);
        case MOOD_LISTENING: return hex565(0x7A6A9A);
        case MOOD_BREAK: return hex565(0x6A8F5A);
        case MOOD_SLEEPING: return hex565(0x3A3A68);
        case MOOD_RELAXING:
        default: return hex565(0xE29A78);
    }
}

static void drawMascot(Canvas& canvas, Mood mood, int cx, int cy) {
    const uint16_t skin = hex565(0xF3C7A6);
    const uint16_t hair = hex565(0x3A2A44);
    const uint16_t cloth = clothFor(mood);
    const bool asleep = mood == MOOD_SLEEPING;

    if (asleep) {
        canvas.fillRoundRect(cx - 30, cy - 6, 60, 24, 10, cloth);
        canvas.fillCircle(cx - 24, cy - 2, 12, skin);
        canvas.fillCircle(cx - 24, cy - 10, 10, hair);
        canvas.drawLine(cx - 30, cy - 4, cx - 24, cy - 4, hair);
        canvas.drawLine(cx - 22, cy - 4, cx - 16, cy - 4, hair);
        canvas.setFont(&fonts::Font2);
        canvas.setTextColor(hex565(0xCDB4DB));
        canvas.setTextDatum(textdatum_t::middle_center);
        canvas.drawString("z", cx + 22, cy - 16);
        return;
    }

    canvas.fillRoundRect(cx - 16, cy - 4, 32, 34, 10, cloth);
    canvas.fillCircle(cx, cy - 28, 16, skin);
    canvas.fillCircle(cx, cy - 36, 14, hair);
    canvas.fillCircle(cx - 6, cy - 28, 2, hair);
    canvas.fillCircle(cx + 6, cy - 28, 2, hair);

    if (mood == MOOD_STUDYING) {
        canvas.fillRoundRect(cx - 12, cy + 8, 24, 16, 2, hex565(0xF4E4C4));
        canvas.drawLine(cx, cy + 8, cx, cy + 24, hex565(0xC4A574));
    } else if (mood == MOOD_WORKING) {
        canvas.fillRect(cx - 18, cy + 18, 36, 4, hex565(0x5A4638));
        canvas.fillRoundRect(cx - 10, cy + 6, 20, 12, 2, hex565(0xD7E4F2));
    } else if (mood == MOOD_LISTENING) {
        canvas.drawCircle(cx - 16, cy - 28, 6, hair);
        canvas.drawCircle(cx + 16, cy - 28, 6, hair);
        canvas.drawLine(cx - 16, cy - 34, cx + 16, cy - 34, hair);
    } else if (mood == MOOD_BREAK || mood == MOOD_RELAXING) {
        canvas.fillRoundRect(cx + 20, cy + 4, 10, 12, 3, hex565(0xF7F1E6));
        canvas.fillRect(cx + 22, cy + 2, 6, 3, hex565(0xE07A5F));
    }
}

void moodDraw(Canvas& canvas, const SkyLook& look, const AppState& state, uint32_t nowMs) {
    skyDraw(canvas, look, state, -1, nowMs, 96);
    canvas.fillRect(0, 88, LPC_SCREEN_WIDTH, 200, look.plate);

    int bob = (state.mood == MOOD_SLEEPING) ? 0 : (((nowMs / 500) % 2) ? 0 : 2);
    drawMascot(canvas, state.mood, 120, 176 + bob);

    canvas.setFont(&fonts::Font4);
    canvas.setTextSize(1);
    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setTextColor(look.ink);
    canvas.drawString(moodName(state.mood), 120, 246);

    canvas.setFont(&fonts::Font2);
    canvas.setTextColor(dim565(look.ink, 2, 3));
    canvas.drawString("tap to change", 120, 272);
}
