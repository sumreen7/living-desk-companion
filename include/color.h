#pragma once

#include <stdint.h>

inline uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

inline uint16_t hex565(uint32_t hex) {
    return rgb565((hex >> 16) & 255, (hex >> 8) & 255, hex & 255);
}

inline uint16_t lerp565(uint16_t a, uint16_t b, uint8_t t) {
    int ar = (a >> 11) & 31;
    int ag = (a >> 5) & 63;
    int ab = a & 31;
    int br = (b >> 11) & 31;
    int bg = (b >> 5) & 63;
    int bb = b & 31;
    int r = ar + (br - ar) * t / 255;
    int g = ag + (bg - ag) * t / 255;
    int bl = ab + (bb - ab) * t / 255;
    return (uint16_t)((r << 11) | (g << 5) | bl);
}

inline uint16_t dim565(uint16_t c, uint8_t num, uint8_t den) {
    int r = ((c >> 11) & 31) * num / den;
    int g = ((c >> 5) & 63) * num / den;
    int b = (c & 31) * num / den;
    return (uint16_t)((r << 11) | (g << 5) | b);
}
