#include "ui.h"

#include <stdio.h>
#include <string.h>

#include "clock.h"
#include "color.h"
#include "config.h"
#include "display.h"
#include "messages.h"
#include "mood.h"
#include "music.h"
#include "navigation.h"
#include "settings.h"
#include "sky.h"
#include "weather.h"

static int wholeDegrees(int tenthsC) {
    int tenths = tenthsC;
    if (settings().tempUnit != 'C') {
        tenths = tenthsC * 9 / 5 + 320;
    }
    if (tenths >= 0) {
        return (tenths + 5) / 10;
    }
    return -(((-tenths) + 5) / 10);
}

static void drawWrapped(Canvas& canvas, const char* text, int cx, int y, int maxWidth, int lineH) {
    if (canvas.textWidth(text) <= maxWidth) {
        canvas.drawString(text, cx, y);
        return;
    }
    char line[80];
    int best = -1;
    int len = (int)strlen(text);
    if (len > 78) {
        len = 78;
    }
    for (int i = 0; i < len; i++) {
        if (text[i] != ' ') {
            continue;
        }
        memcpy(line, text, i);
        line[i] = '\0';
        if (canvas.textWidth(line) <= maxWidth) {
            best = i;
        }
    }
    if (best < 0) {
        canvas.drawString(text, cx, y);
        return;
    }
    memcpy(line, text, best);
    line[best] = '\0';
    canvas.drawString(line, cx, y);
    canvas.drawString(text + best + 1, cx, y + lineH);
}

static void drawDots(Canvas& canvas, Page page, uint16_t ink) {
    const int activeW = 16;
    const int idle = 5;
    const int gap = 8;
    int width = activeW + idle * 3 + gap * 3;
    int x = (LPC_SCREEN_WIDTH - width) / 2;
    uint16_t quiet = dim565(ink, 1, 2);
    for (int i = 0; i < PAGE_COUNT; i++) {
        if ((Page)i == page) {
            canvas.fillRoundRect(x, 306, activeW, 6, 3, ink);
            x += activeW + gap;
        } else {
            canvas.fillCircle(x + 2, 309, 2, quiet);
            x += idle + gap;
        }
    }
}

static void drawHome(Canvas& canvas, const AppState& state, const SkyLook& look, const struct tm* nowTm, int minute, uint32_t nowMs) {
    skyDraw(canvas, look, state, minute, nowMs, LPC_SCREEN_HEIGHT);

    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setFont(&fonts::Font4);
    canvas.setTextSize(2);
    char clock[16];
    if (nowTm) {
        clockFormatTime(clock, sizeof(clock), *nowTm, false);
    } else {
        snprintf(clock, sizeof(clock), "--:--");
    }
    canvas.setTextColor(look.hill);
    canvas.drawString(clock, 121, 186);
    canvas.setTextColor(look.clock);
    canvas.drawString(clock, 120, 185);
    canvas.setTextSize(1);

    if (nowTm) {
        char date[32];
        clockFormatDate(date, sizeof(date), *nowTm);
        if (settings().clock12h) {
            char withPeriod[40];
            snprintf(withPeriod, sizeof(withPeriod), "%s  %s", date, nowTm->tm_hour >= 12 ? "PM" : "AM");
            strncpy(date, withPeriod, sizeof(date) - 1);
            date[sizeof(date) - 1] = '\0';
        }
        canvas.setFont(&fonts::Font2);
        canvas.setTextColor(look.clock);
        canvas.drawString(date, 120, 214);
    }

    canvas.fillRoundRect(16, 236, 208, 54, 14, look.plate);
    canvas.setFont(&fonts::Font2);
    canvas.setTextColor(look.ink);
    canvas.setTextDatum(textdatum_t::top_center);
    const char* message = "Learning the time...";
    if (settings().ssid[0] == '\0') {
        message = "Add Wi-Fi in secrets.h";
    } else if (nowTm) {
        message = messageFor(look.period, nowMs);
    }
    drawWrapped(canvas, message, 120, 250, 184, 16);
}

static void drawWeather(Canvas& canvas, const AppState& state, const SkyLook& look, int minute, uint32_t nowMs) {
    skyDraw(canvas, look, state, minute, nowMs, 132);
    canvas.fillRoundRect(12, 118, 216, 176, 16, look.plate);
    canvas.setTextDatum(textdatum_t::middle_center);
    canvas.setTextColor(look.ink);

    if (settings().city[0] == '\0') {
        canvas.setFont(&fonts::Font2);
        drawWrapped(canvas, "Add your city in secrets.h", 120, 180, 180, 18);
        return;
    }
    if (!state.weatherValid) {
        canvas.setFont(&fonts::Font2);
        const char* message = state.note[0] ? state.note : "Asking the sky...";
        drawWrapped(canvas, message, 120, 180, 180, 18);
        return;
    }

    char number[8];
    snprintf(number, sizeof(number), "%d", wholeDegrees(state.tempTenthsC));
    canvas.setFont(&fonts::Font4);
    canvas.setTextSize(2);
    int center = canvas.drawString(number, 108, 168);
    canvas.setTextSize(1);
    canvas.setFont(&fonts::Font2);
    canvas.drawString(settings().tempUnit == 'C' ? "C" : "F", 108 + center / 2 + 14, 154);
    canvas.drawCircle(108 + center / 2 + 6, 146, 3, look.ink);

    canvas.setFont(&fonts::Font4);
    canvas.setTextSize(1);
    canvas.drawString(state.condition, 120, 206);
    canvas.setFont(&fonts::Font2);
    canvas.drawString(state.place, 120, 232);

    if (state.highTenthsC != INT16_MIN && state.lowTenthsC != INT16_MIN) {
        char band[32];
        snprintf(band, sizeof(band), "%d  /  %d", wholeDegrees(state.highTenthsC), wholeDegrees(state.lowTenthsC));
        canvas.drawString(band, 120, 258);
    }
    if (!state.wifiConnected) {
        canvas.setTextColor(dim565(look.ink, 2, 3));
        canvas.drawString("saved", 120, 278);
    }
}

void uiHandleGesture(const Gesture& gesture) {
    if (gesture.kind == GESTURE_SWIPE_LEFT) {
        navigationNudge(1);
        return;
    }
    if (gesture.kind == GESTURE_SWIPE_RIGHT) {
        navigationNudge(-1);
        return;
    }
    if (gesture.kind != GESTURE_TAP) {
        return;
    }
    AppState state;
    stateSnapshot(state);
    if (state.page == PAGE_MUSIC) {
        musicToggle();
    } else if (state.page == PAGE_MOOD) {
        moodCycle();
    } else if (state.page == PAGE_SKY) {
        messageAdvance();
    } else if (state.page == PAGE_WEATHER) {
        weatherRequestSoon();
    }
}

void uiHandleKey(char key) {
    if (key == 'd' || key == 'D' || key == 'n') {
        navigationNudge(1);
    } else if (key == 'a' || key == 'A' || key == 'p') {
        navigationNudge(-1);
    } else if (key == ' ') {
        musicToggle();
    } else if (key == 'm' || key == 'M') {
        messageAdvance();
    } else if (key >= '1' && key <= '6') {
        moodSet((Mood)(key - '1'));
    } else if (key == 'r' || key == 'R') {
        weatherRequestSoon();
    }
}

void uiRender(uint32_t nowMs) {
    AppState state;
    stateSnapshot(state);
    struct tm nowTm;
    const bool hasTime = clockRead(nowTm);
    const int minute = hasTime ? clockMinuteOfDay(nowTm) : -1;
    const SkyLook look = skyLookFor(state, minute);

    Canvas page = displayCanvas(navigationShift());
    page.fillScreen(look.bottom);
    if (state.page == PAGE_SKY) {
        drawHome(page, state, look, hasTime ? &nowTm : nullptr, minute, nowMs);
    } else if (state.page == PAGE_WEATHER) {
        drawWeather(page, state, look, minute, nowMs);
    } else if (state.page == PAGE_MUSIC) {
        musicDraw(page, look, state, nowMs);
    } else {
        moodDraw(page, look, state, nowMs);
    }

    Canvas hud = displayCanvas(0);
    drawDots(hud, state.page, look.clock);
    displayPushFrame();
}
