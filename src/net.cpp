#include "net.h"

#include <Arduino.h>
#include <WiFi.h>
#include <string.h>
#include <time.h>

#include "app_state.h"
#include "log.h"
#include "settings.h"
#include "weather.h"

static char gAppliedTz[64] = "";
static bool gAnnounced = false;

static void applyTimezone() {
    const char* tz = weatherSavedTz();
    if (!tz || tz[0] == '\0' || strcmp(tz, gAppliedTz) == 0) {
        return;
    }
    configTzTime(tz, "pool.ntp.org", "time.nist.gov");
    strncpy(gAppliedTz, tz, sizeof(gAppliedTz) - 1);
    gAppliedTz[sizeof(gAppliedTz) - 1] = '\0';
    logLine("clock: timezone set");
}

static void netTask(void*) {
    const UserSettings& user = settings();
    uint32_t lastBegin = 0;
    if (user.ssid[0] == '\0') {
        logLine("wifi: add your network in include/secrets.h");
    }

    for (;;) {
        if (user.ssid[0] == '\0') {
            StateGuard guard;
            guard.get().wifiConnected = false;
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (WiFi.status() != WL_CONNECTED) {
            gAnnounced = false;
            {
                StateGuard guard;
                guard.get().wifiConnected = false;
            }
            uint32_t now = millis();
            if (lastBegin == 0 || (now - lastBegin) > 20000) {
                logf("wifi: connecting to %s", user.ssid);
                WiFi.disconnect();
                WiFi.begin(user.ssid, user.password);
                lastBegin = now;
            }
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        if (!gAnnounced) {
            gAnnounced = true;
            logLine("wifi: connected");
        }
        {
            StateGuard guard;
            guard.get().wifiConnected = true;
            guard.get().timeValid = time(nullptr) > 1700000000;
        }
        applyTimezone();
        weatherMaintain(millis());
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void netBegin() {
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("living-pixel");
    WiFi.setAutoReconnect(true);
    BaseType_t ok = xTaskCreatePinnedToCore(netTask, "net", 32768, nullptr, 1, nullptr, 0);
    if (ok != pdPASS) {
        logLine("wifi: background task did not start");
    }
}
