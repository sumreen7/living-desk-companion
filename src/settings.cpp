#include "settings.h"

#include <string.h>

#include "config.h"

static UserSettings gSettings;

static void copyText(char* dest, size_t n, const char* src) {
    if (n == 0) {
        return;
    }
    strncpy(dest, src, n - 1);
    dest[n - 1] = '\0';
}

void settingsBegin() {
    memset(&gSettings, 0, sizeof(gSettings));
    copyText(gSettings.ssid, sizeof(gSettings.ssid), LPC_WIFI_SSID);
    copyText(gSettings.password, sizeof(gSettings.password), LPC_WIFI_PASSWORD);
    copyText(gSettings.city, sizeof(gSettings.city), LPC_CITY);
    copyText(gSettings.tzPosix, sizeof(gSettings.tzPosix), LPC_TZ_POSIX);
    gSettings.tempUnit = (LPC_TEMP_UNIT == 'C') ? 'C' : 'F';
    gSettings.clock12h = LPC_CLOCK_12H != 0;
}

const UserSettings& settings() {
    return gSettings;
}
