#pragma once

// Copy this file to include/secrets.h and fill in the two Wi-Fi lines.
// secrets.h is ignored by git, so the password is not uploaded to GitHub.
//
// The ESP32 can only join 2.4 GHz Wi-Fi. If your network name ends in "5G",
// use the other name. A single shared name usually still has a 2.4 GHz band.

#define LPC_WIFI_SSID ""
#define LPC_WIFI_PASSWORD ""

// Your city, as you would say it to a person. If the name is common, add
// the region: "London, Canada" or "Portland, Oregon".
// No weather account is required.
#define LPC_CITY ""

// Clock timezone. Leave this line for US Eastern time (New York, Toronto,
// Detroit, and most of the east coast). Otherwise replace it with one line
// from the list, or set it to "" and the city lookup will choose.
//
// US Central     "CST6CDT,M3.2.0,M11.1.0"
// US Mountain    "MST7MDT,M3.2.0,M11.1.0"
// Arizona        "MST7"
// US Pacific     "PST8PDT,M3.2.0,M11.1.0"
// UK             "GMT0BST,M3.5.0/1,M10.5.0"
// Central Europe "CET-1CEST,M3.5.0,M10.5.0"
// India          "IST-5:30"
// Japan          "JST-9"
#define LPC_TZ_POSIX "EST5EDT,M3.2.0,M11.1.0"

// 'F' or 'C'
#define LPC_TEMP_UNIT 'C'

// 1 = 2:30 PM, 0 = 14:30
#define LPC_CLOCK_12H 0
