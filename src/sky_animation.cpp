#include "sky.h"

#include <Arduino.h>
#include <math.h>

#include "color.h"
#include "config.h"

namespace {

struct Mark {
    int minute;
    SkyPeriod period;
};

SkyLook lookFor(SkyPeriod period) {
    switch (period) {
        case SKY_EARLY_MORNING:
            return {hex565(0x1B2440), hex565(0x6E5A86), hex565(0xF0C9A0), hex565(0xE4D7F2),
                    hex565(0xC9B8D4), hex565(0x3A3158), hex565(0xF6EDE4), hex565(0x2C3148),
                    hex565(0xF8F1E6), true, true, false, period};
        case SKY_SUNRISE:
            return {hex565(0xF7B267), hex565(0xF4A3B5), hex565(0xFFE08A), hex565(0xFFE08A),
                    hex565(0xFFD9C8), hex565(0xC46B6B), hex565(0xFFE9D6), hex565(0x5A3340),
                    hex565(0xFFF6EA), false, true, false, period};
        case SKY_MORNING:
            return {hex565(0x6EB6FF), hex565(0xC9E7FF), hex565(0xFFE27A), hex565(0xFFF8E8),
                    hex565(0xFFFEFF), hex565(0x5E8FBE), hex565(0xFFF6E8), hex565(0x243044),
                    hex565(0x1B3358), false, true, false, period};
        case SKY_AFTERNOON:
            return {hex565(0x3E86F0), hex565(0x9FD0FF), hex565(0xFFD95A), hex565(0xFFF6D0),
                    hex565(0xF7FBFF), hex565(0x3E6E9E), hex565(0xFFF6E8), hex565(0x243044),
                    hex565(0x16305A), false, true, false, period};
        case SKY_GOLDEN_HOUR:
            return {hex565(0xF6C48A), hex565(0xF09A62), hex565(0xFFE08A), hex565(0xFFE08A),
                    hex565(0xFFE6C7), hex565(0xC47A55), hex565(0xFFE7C4), hex565(0x5A3424),
                    hex565(0x4A2C1C), false, true, false, period};
        case SKY_SUNSET:
            return {hex565(0x6A3A78), hex565(0xF08A5D), hex565(0xFFB15A), hex565(0xF4D7C4),
                    hex565(0xF6C1B0), hex565(0x8A3E55), hex565(0x3A2744), hex565(0xF8E6D8),
                    hex565(0xFFE9D2), false, true, false, period};
        case SKY_DUSK:
            return {hex565(0x2A2150), hex565(0x6A5588), hex565(0xE8B48A), hex565(0xF3E9D2),
                    hex565(0x8E7EAE), hex565(0x322848), hex565(0x241C3A), hex565(0xE6DDF2),
                    hex565(0xF3E9F8), true, false, true, period};
        case SKY_NIGHT:
        default:
            return {hex565(0x0C1228), hex565(0x1C2A52), hex565(0xF4F0E0), hex565(0xF4F0E0),
                    hex565(0x2A3A66), hex565(0x12182E), hex565(0x1A2038), hex565(0xE4DCC8),
                    hex565(0xF4EBD8), true, false, true, period};
    }
}

SkyLook mixLook(const SkyLook& a, const SkyLook& b, uint8_t t) {
    if (t == 0) {
        return a;
    }
    SkyLook out = b;
    out.top = lerp565(a.top, b.top, t);
    out.bottom = lerp565(a.bottom, b.bottom, t);
    out.sun = lerp565(a.sun, b.sun, t);
    out.moon = lerp565(a.moon, b.moon, t);
    out.cloud = lerp565(a.cloud, b.cloud, t);
    out.hill = lerp565(a.hill, b.hill, t);
    out.plate = lerp565(a.plate, b.plate, t);
    out.ink = lerp565(a.ink, b.ink, t);
    out.clock = lerp565(a.clock, b.clock, t);
    if (t < 200) {
        out.stars = a.stars;
        out.showSun = a.showSun;
        out.showMoon = a.showMoon;
        out.period = a.period;
    }
    return out;
}

int buildMarks(const AppState& state, Mark* marks) {
    int sr = state.sunriseMin;
    int ss = state.sunsetMin;
    if (sr < 0 || ss < sr + 145) {
        sr = 6 * 60 + 20;
        ss = 18 * 60;
    }
    const int day = ss - sr;
    const int planned[][2] = {
        {0, SKY_NIGHT},
        {sr - 60, SKY_EARLY_MORNING},
        {sr - 10, SKY_SUNRISE},
        {sr + 50, SKY_MORNING},
        {sr + day * 45 / 100, SKY_AFTERNOON},
        {ss - 80, SKY_GOLDEN_HOUR},
        {ss - 20, SKY_SUNSET},
        {ss + 15, SKY_DUSK},
        {ss + 75, SKY_NIGHT},
    };
    int count = 0;
    int cursor = -1;
    for (int i = 0; i < 9; i++) {
        int minute = planned[i][0];
        if (minute < 0) {
            minute = 0;
        }
        if (minute > 1439) {
            minute = 1439;
        }
        if (minute <= cursor) {
            continue;
        }
        marks[count].minute = minute;
        marks[count].period = (SkyPeriod)planned[i][1];
        cursor = minute;
        count++;
    }
    return count;
}

void applyWeather(SkyLook& look, WeatherKind weather) {
    uint16_t toward = look.top;
    uint8_t amount = 0;
    bool hideSun = false;
    bool hideMoon = false;
    bool hideStars = false;
    switch (weather) {
        case WEATHER_CLOUDY:
            toward = hex565(0x96A8B2);
            amount = 70;
            hideSun = true;
            hideStars = true;
            break;
        case WEATHER_FOG:
            toward = hex565(0xC8CCCE);
            amount = 120;
            hideSun = true;
            hideMoon = true;
            hideStars = true;
            break;
        case WEATHER_RAIN:
            toward = hex565(0x465060);
            amount = 100;
            hideSun = true;
            hideMoon = true;
            hideStars = true;
            break;
        case WEATHER_SNOW:
            toward = hex565(0xAEC6D8);
            amount = 80;
            hideSun = true;
            hideMoon = true;
            hideStars = true;
            break;
        case WEATHER_STORM:
            toward = hex565(0x1C2030);
            amount = 150;
            hideSun = true;
            hideMoon = true;
            hideStars = true;
            break;
        default:
            break;
    }
    if (amount) {
        look.top = lerp565(look.top, toward, amount);
        look.bottom = lerp565(look.bottom, toward, amount);
        look.cloud = lerp565(look.cloud, toward, amount / 2);
        look.hill = lerp565(look.hill, toward, amount);
    }
    if (hideSun) {
        look.showSun = false;
    }
    if (hideMoon) {
        look.showMoon = false;
    }
    if (hideStars) {
        look.stars = false;
    }
}

void fillGradient(Canvas& canvas, int height, uint16_t top, uint16_t bottom) {
    const int bands = 20;
    for (int i = 0; i < bands; i++) {
        int y0 = height * i / bands;
        int y1 = height * (i + 1) / bands;
        uint8_t t = (uint8_t)(i * 255 / (bands - 1));
        canvas.fillRect(0, y0, LPC_SCREEN_WIDTH, y1 - y0, lerp565(top, bottom, t));
    }
}

void drawSun(Canvas& canvas, int cx, int cy, uint16_t color, uint32_t nowMs) {
    canvas.fillCircle(cx, cy, 14, color);
    canvas.fillCircle(cx, cy, 8, hex565(0xFFF6C8));
    float spin = (nowMs % 20000) / 20000.0f * 6.28318f;
    for (int i = 0; i < 8; i++) {
        float a = spin + i * 6.28318f / 8.0f;
        int x0 = cx + (int)(cosf(a) * 18);
        int y0 = cy + (int)(sinf(a) * 18);
        int x1 = cx + (int)(cosf(a) * 26);
        int y1 = cy + (int)(sinf(a) * 26);
        canvas.drawLine(x0, y0, x1, y1, color);
    }
}

void drawMoon(Canvas& canvas, int cx, int cy, uint16_t color) {
    canvas.fillCircle(cx, cy, 12, color);
    canvas.fillCircle(cx - 4, cy - 3, 2, dim565(color, 3, 4));
    canvas.fillCircle(cx + 4, cy + 3, 2, dim565(color, 3, 4));
}

void drawCloud(Canvas& canvas, int x, int y, uint16_t color, int scale) {
    canvas.fillEllipse(x, y, 16 * scale / 10, 8, color);
    canvas.fillEllipse(x + 14, y + 2, 12 * scale / 10, 7, color);
    canvas.fillEllipse(x - 12, y + 2, 10 * scale / 10, 6, color);
}

void drawHills(Canvas& canvas, int baseline, uint16_t color) {
    canvas.fillCircle(28, baseline + 18, 28, color);
    canvas.fillCircle(78, baseline + 24, 36, color);
    canvas.fillCircle(138, baseline + 20, 30, color);
    canvas.fillCircle(198, baseline + 26, 40, color);
    canvas.fillRect(0, baseline + 10, LPC_SCREEN_WIDTH, 40, color);
}

void drawBird(Canvas& canvas, int x, int y, uint16_t color) {
    canvas.fillCircle(x, y, 3, color);
    canvas.drawLine(x - 6, y, x, y - 2, color);
    canvas.drawLine(x, y - 2, x + 7, y + 1, color);
}

}  // namespace

SkyLook skyLookFor(const AppState& state, int minuteOfDay) {
    if (minuteOfDay < 0) {
        SkyLook welcome = lookFor(SKY_MORNING);
        applyWeather(welcome, state.weather);
        return welcome;
    }

    Mark marks[9];
    int count = buildMarks(state, marks);
    int index = 0;
    for (int i = 0; i < count; i++) {
        if (minuteOfDay >= marks[i].minute) {
            index = i;
        }
    }
    SkyPeriod current = marks[index].period;
    SkyPeriod next = current;
    int nextMinute = 1440;
    if (index + 1 < count) {
        next = marks[index + 1].period;
        nextMinute = marks[index + 1].minute;
    }
    uint8_t blend = 0;
    int distance = nextMinute - minuteOfDay;
    if (distance >= 0 && distance <= 20) {
        blend = (uint8_t)((20 - distance) * 255 / 20);
    }
    SkyLook look = mixLook(lookFor(current), lookFor(next), blend);
    applyWeather(look, state.weather);
    return look;
}

void skyDraw(Canvas& canvas, const SkyLook& look, const AppState& state, int minuteOfDay, uint32_t nowMs, int skyHeight) {
    if (skyHeight < 20) {
        skyHeight = 20;
    }
    if (skyHeight > LPC_SCREEN_HEIGHT) {
        skyHeight = LPC_SCREEN_HEIGHT;
    }
    fillGradient(canvas, skyHeight, look.top, look.bottom);

    if (look.stars && skyHeight > 80) {
        static const int kStars[][2] = {
            {16, 18}, {42, 34}, {70, 14}, {96, 42}, {128, 22}, {160, 16},
            {188, 38}, {214, 20}, {30, 62}, {84, 70}, {150, 58}, {206, 66},
            {54, 92}, {176, 88}, {112, 48}, {224, 48},
        };
        for (int i = 0; i < 16; i++) {
            if (kStars[i][1] > skyHeight - 24) {
                continue;
            }
            uint8_t twinkle = (uint8_t)((nowMs / 180 + i * 3) % 5);
            uint16_t color = twinkle == 0 ? dim565(look.moon, 1, 2) : look.moon;
            canvas.fillRect(kStars[i][0], kStars[i][1], twinkle == 1 ? 3 : 2, 2, color);
        }
    }

    int sr = state.sunriseMin >= 0 ? state.sunriseMin : 6 * 60 + 20;
    int ss = state.sunsetMin >= 0 ? state.sunsetMin : 18 * 60;
    if (ss <= sr) {
        ss = sr + 12 * 60;
    }
    int minute = minuteOfDay >= 0 ? minuteOfDay : sr + (ss - sr) / 3;
    int horizon = skyHeight - 16;
    int radius = skyHeight > 220 ? 96 : skyHeight / 3;

    if (look.showSun && minute >= sr - 20 && minute <= ss + 20) {
        float p = (float)(minute - sr) / (float)(ss - sr);
        if (p < 0) {
            p = 0;
        }
        if (p > 1) {
            p = 1;
        }
        int cx = 24 + (int)(p * (LPC_SCREEN_WIDTH - 48));
        int cy = horizon - (int)(sinf(p * 3.14159f) * radius);
        drawSun(canvas, cx, cy, look.sun, nowMs);
    } else if (look.showMoon) {
        int cx = 168;
        int cy = skyHeight > 180 ? 58 : 36;
        drawMoon(canvas, cx, cy, look.moon);
    }

    int clouds = 3;
    int scale = 10;
    switch (state.weather) {
        case WEATHER_CLEAR: clouds = 2; scale = 8; break;
        case WEATHER_PARTLY: clouds = 3; scale = 10; break;
        case WEATHER_CLOUDY: clouds = 5; scale = 12; break;
        case WEATHER_FOG: clouds = 0; break;
        case WEATHER_RAIN: clouds = 4; scale = 12; break;
        case WEATHER_SNOW: clouds = 3; scale = 11; break;
        case WEATHER_STORM: clouds = 4; scale = 13; break;
        default: break;
    }
    for (int i = 0; i < clouds; i++) {
        int speed = 90 + i * 30;
        int x = (int)((nowMs / (uint32_t)speed + i * 97) % 320) - 50;
        int y = 26 + (i * 18) % (skyHeight > 80 ? skyHeight / 3 : 30);
        drawCloud(canvas, x, y, look.cloud, scale);
    }

    if (state.weather == WEATHER_FOG) {
        uint16_t fog = lerp565(look.cloud, look.plate, 140);
        for (int i = 0; i < 3; i++) {
            int x = (int)((nowMs / 120 + i * 80) % 280) - 20;
            canvas.fillEllipse(x, skyHeight / 3 + i * 16, 70, 10, fog);
        }
    } else if (state.weather == WEATHER_RAIN || state.weather == WEATHER_STORM) {
        uint16_t drop = hex565(0xD5E4F4);
        int count = state.weather == WEATHER_STORM ? 32 : 22;
        for (int i = 0; i < count; i++) {
            int x = (i * 37 + (int)(nowMs / 10)) % (LPC_SCREEN_WIDTH + 20) - 10;
            int y = (i * 53 + (int)(nowMs / 6)) % (skyHeight + 16) - 8;
            canvas.drawLine(x, y, x - 2, y + 8, drop);
        }
    } else if (state.weather == WEATHER_SNOW) {
        for (int i = 0; i < 18; i++) {
            int x = (i * 29 + (int)(nowMs / 30)) % (LPC_SCREEN_WIDTH + 10) - 5;
            int y = (i * 47 + (int)(nowMs / 24)) % (skyHeight + 10);
            canvas.fillCircle(x, y, (i % 3) == 0 ? 2 : 1, hex565(0xFFFBFF));
        }
    }

    if (state.weather == WEATHER_STORM && skyHeight > 70 && (nowMs % 11000) < 110) {
        int x = 40 + (int)((nowMs / 11000) % 140);
        canvas.drawLine(x, 8, x - 10, 36, hex565(0xFFF8E8));
        canvas.drawLine(x - 10, 36, x + 4, 36, hex565(0xFFF8E8));
        canvas.drawLine(x + 4, 36, x - 8, 70, hex565(0xFFF8E8));
    }

    if (skyHeight > 200 && look.stars && state.weather == WEATHER_CLEAR) {
        static uint32_t shotStart = 0;
        static int shotX = 0;
        static int shotY = 0;
        static bool shotOn = false;
        static uint32_t nextShot = 8000;
        if (!shotOn && nowMs > nextShot) {
            shotOn = true;
            shotStart = nowMs;
            shotX = 20 + (int)(esp_random() % 140);
            shotY = 12 + (int)(esp_random() % 40);
            nextShot = nowMs + 18000 + (esp_random() % 16000);
        }
        if (shotOn) {
            uint32_t dt = nowMs - shotStart;
            if (dt > 420) {
                shotOn = false;
            } else {
                int x = shotX + (int)dt / 3;
                int y = shotY + (int)dt / 6;
                canvas.drawLine(x - 10, y - 4, x, y, look.moon);
            }
        }
    }

    if (skyHeight > 180) {
        drawHills(canvas, skyHeight > 250 ? 250 : skyHeight - 28, look.hill);
        if (look.showSun && (state.weather == WEATHER_CLEAR || state.weather == WEATHER_PARTLY || state.weather == WEATHER_UNKNOWN)) {
            int bob = ((nowMs / 450) % 2) ? 0 : 2;
            int x = 48 + (int)((nowMs / 80) % 90);
            drawBird(canvas, x, (skyHeight > 250 ? 242 : skyHeight - 36) - bob, dim565(look.ink, 2, 3));
        }
    }
}
