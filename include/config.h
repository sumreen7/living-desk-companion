#pragma once

#if __has_include("secrets.h")
#include "secrets.h"
#endif

// Defaults apply when include/secrets.h does not override them.
// Pittsburgh uses US Eastern time. Temperature is Celsius.
// The clock uses 24-hour time, such as 20:26.

#ifndef LPC_WIFI_SSID
#define LPC_WIFI_SSID ""
#endif

#ifndef LPC_WIFI_PASSWORD
#define LPC_WIFI_PASSWORD ""
#endif

#ifndef LPC_CITY
#define LPC_CITY ""
#endif

#ifndef LPC_TZ_POSIX
#define LPC_TZ_POSIX "EST5EDT,M3.2.0,M11.1.0"
#endif

#ifndef LPC_TEMP_UNIT
#define LPC_TEMP_UNIT 'C'
#endif

#ifndef LPC_CLOCK_12H
#define LPC_CLOCK_12H 0
#endif

#define LPC_PROJECT_NAME "Living Pixel Companion"
#define LPC_SCREEN_WIDTH 240
#define LPC_SCREEN_HEIGHT 320

// 0 = as wired. 2 = upside down. Portrait only.
#ifndef LPC_ROTATION
#define LPC_ROTATION 0
#endif

// Set to 1 if the colour bars look like a photo negative.
#ifndef LPC_INVERT_COLORS
#define LPC_INVERT_COLORS 0
#endif

// Set to 1 if red and blue are swapped.
#ifndef LPC_BGR_PANEL
#define LPC_BGR_PANEL 0
#endif

// Jumper wires are happier at this speed than at the panel's maximum.
#define LPC_SPI_WRITE_HZ 27000000
#define LPC_SPI_READ_HZ 16000000
#define LPC_TOUCH_HZ 2500000

#define LPC_FRAME_MS 100
#define LPC_WEATHER_OK_MS (20UL * 60UL * 1000UL)
#define LPC_WEATHER_RETRY_MS (2UL * 60UL * 1000UL)
#define LPC_MESSAGE_MS (3UL * 60UL * 1000UL)
