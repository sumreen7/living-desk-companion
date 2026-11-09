#pragma once

#include <stdint.h>

struct UserSettings {
    char ssid[33];
    char password[65];
    char city[40];
    char tzPosix[64];
    char tempUnit;
    bool clock12h;
};

void settingsBegin();
const UserSettings& settings();
