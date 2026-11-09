#include "log.h"

#include <Arduino.h>
#include <stdio.h>
#include <stdarg.h>

void logBegin() {
    Serial.begin(115200);
    Serial0.begin(115200);
    // USB serial must not wait forever when the gadget is on a plain charger.
    Serial.setTxTimeoutMs(0);

    const uint32_t start = millis();
    while (!Serial && (millis() - start) < 2000) {
        delay(10);
    }
}

void logLine(const char* line) {
    Serial.println(line);
    Serial0.println(line);
}

void logf(const char* fmt, ...) {
    char buf[180];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    logLine(buf);
}
