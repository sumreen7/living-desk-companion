#pragma once

// Planned wiring for an ESP32-S3 N16R8 and the common 14-pin 2.4-inch SPI panel.
// These GPIO numbers avoid the pins that the module uses for its own memory,
// USB sockets, and boot mode.
//
// Before connecting a wire, the same number must be printed on the ESP32
// header (10, IO10, and GPIO10 are the same pin). If a number is missing,
// stop and send a photo. Do not pick a different pin yourself.

#define PIN_TFT_SCK 12
#define PIN_TFT_MOSI 11
#define PIN_TFT_MISO 13
#define PIN_TFT_CS 10
#define PIN_TFT_DC 9
#define PIN_TFT_RST 14

// The backlight is powered from the 3.3 V rail, not from a GPIO.
// A data pin cannot comfortably supply the backlight's current.
#define PIN_TFT_BL -1

#define PIN_TOUCH_CS 4
#define PIN_TOUCH_IRQ 5

// Raw touch range for the usual XPT2046-compatible chip on this panel.
// If a tap lands in a mirrored spot, swap the matching MIN and MAX.
#define TOUCH_X_MIN 300
#define TOUCH_X_MAX 3900
#define TOUCH_Y_MIN 300
#define TOUCH_Y_MAX 3900
#define TOUCH_ROTATION 0
