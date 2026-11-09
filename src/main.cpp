#include <Arduino.h>

#include "app_state.h"
#include "clock.h"
#include "config.h"
#include "display.h"
#include "log.h"
#include "navigation.h"
#include "net.h"
#include "settings.h"
#include "touch.h"
#include "ui.h"
#include "weather.h"

static void printIdentity() {
    const uint32_t flashBytes = ESP.getFlashChipSize();
    const uint32_t psramBytes = ESP.getPsramSize();
    const uint64_t mac = ESP.getEfuseMac();
    logLine("================================");
    logLine(LPC_PROJECT_NAME);
    logLine("Ready for the desk, waiting on hardware.");
    logLine("================================");
    logf("Chip: %s  rev %d  %u MHz", ESP.getChipModel(), ESP.getChipRevision(), ESP.getCpuFreqMHz());
    logf("Flash: %u MB", flashBytes / (1024u * 1024u));
    logf("PSRAM: %u MB", psramBytes / (1024u * 1024u));
    logf("Free RAM: %u    free PSRAM: %u", ESP.getFreeHeap(), psramBytes ? ESP.getFreePsram() : 0);
    logf("MAC: %02X:%02X:%02X:%02X:%02X:%02X",
         (unsigned)((mac >> 0) & 0xFF), (unsigned)((mac >> 8) & 0xFF),
         (unsigned)((mac >> 16) & 0xFF), (unsigned)((mac >> 24) & 0xFF),
         (unsigned)((mac >> 32) & 0xFF), (unsigned)((mac >> 40) & 0xFF));
    if (flashBytes < 16u * 1024u * 1024u || psramBytes < 7u * 1024u * 1024u) {
        logLine("CHECK: flash or PSRAM does not match an N16R8 board. Tell me this line.");
    } else {
        logLine("CHECK: flash and PSRAM look right.");
    }
}

static void pollKeys() {
    while (Serial.available()) {
        uiHandleKey((char)Serial.read());
    }
    while (Serial0.available()) {
        uiHandleKey((char)Serial0.read());
    }
}

void setup() {
    logBegin();
    appStateInit();
    settingsBegin();
    weatherLoad();
    printIdentity();
    logLine("keys: a/d pages, space play, 1-6 mood, m message, r refresh");

    bool sprite = false;
    if (displayStart()) {
        displayColorTest();
        sprite = displayMakeSprite();
    }
    touchBegin();
    netBegin();
    {
        StateGuard guard;
        guard.get().displayReady = sprite;
    }
    if (!sprite) {
        logLine("display: picture area is not ready");
    }
}

void loop() {
    const uint32_t now = millis();
    pollKeys();
    navigationTick(now);
    Gesture gesture = touchPoll(now);
    if (gesture.kind != GESTURE_NONE) {
        uiHandleGesture(gesture);
    }

    static uint32_t lastFrame = 0;
    static uint32_t lastStatus = 0;
    AppState state;
    stateSnapshot(state);

    if (state.displayReady && (now - lastFrame) >= LPC_FRAME_MS) {
        uiRender(now);
        lastFrame = millis();
    }

    if ((now - lastStatus) >= 5000) {
        lastStatus = now;
        struct tm t;
        if (clockRead(t)) {
            char clock[20];
            clockFormatTime(clock, sizeof(clock), t, true);
            logf("%s  wifi %s  weather %s",
                 clock,
                 state.wifiConnected ? "up" : "down",
                 state.weatherValid ? state.condition : "waiting");
        } else if (settings().ssid[0] == '\0') {
            logLine("waiting for Wi-Fi details in include/secrets.h");
        } else {
            logLine("waiting for the clock");
        }
    }
    delay(5);
}
