#pragma once

#include <LovyanGFX.hpp>

// A drawing surface. ox slides a whole page sideways during a swipe.
struct Canvas {
    lgfx::LGFXBase* d;
    int ox;

    void fillScreen(uint16_t c) { d->fillScreen(c); }
    void fillRect(int x, int y, int w, int h, uint16_t c) { d->fillRect(x + ox, y, w, h, c); }
    void fillRoundRect(int x, int y, int w, int h, int r, uint16_t c) {
        d->fillRoundRect(x + ox, y, w, h, r, c);
    }
    void fillCircle(int x, int y, int r, uint16_t c) { d->fillCircle(x + ox, y, r, c); }
    void drawCircle(int x, int y, int r, uint16_t c) { d->drawCircle(x + ox, y, r, c); }
    void fillEllipse(int x, int y, int rx, int ry, uint16_t c) {
        d->fillEllipse(x + ox, y, rx, ry, c);
    }
    void drawLine(int x0, int y0, int x1, int y1, uint16_t c) {
        d->drawLine(x0 + ox, y0, x1 + ox, y1, c);
    }
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t c) {
        d->fillTriangle(x0 + ox, y0, x1 + ox, y1, x2 + ox, y2, c);
    }
    void setFont(const lgfx::IFont* font) { d->setFont(font); }
    void setTextSize(float size) { d->setTextSize(size); }
    void setTextColor(uint16_t c) { d->setTextColor(c); }
    void setTextDatum(textdatum_t datum) { d->setTextDatum(datum); }
    int drawString(const char* text, int x, int y) { return d->drawString(text, x + ox, y); }
    int textWidth(const char* text) { return d->textWidth(text); }
};
