#include "app_state.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <string.h>

static AppState gState;
static SemaphoreHandle_t gLock = nullptr;

void appStateInit() {
    memset(&gState, 0, sizeof(gState));
    gState.page = PAGE_SKY;
    gState.mood = MOOD_RELAXING;
    gState.musicPlaying = true;
    gState.tempTenthsC = INT16_MIN;
    gState.highTenthsC = INT16_MIN;
    gState.lowTenthsC = INT16_MIN;
    gState.sunriseMin = -1;
    gState.sunsetMin = -1;
    gLock = xSemaphoreCreateMutex();
}

StateGuard::StateGuard() {
    if (gLock) {
        xSemaphoreTake(gLock, portMAX_DELAY);
    }
}

StateGuard::~StateGuard() {
    if (gLock) {
        xSemaphoreGive(gLock);
    }
}

AppState& StateGuard::get() {
    return gState;
}

void stateSnapshot(AppState& out) {
    StateGuard guard;
    out = gState;
}
