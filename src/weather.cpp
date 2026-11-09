#include "weather.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app_state.h"
#include "config.h"
#include "log.h"
#include "settings.h"

namespace {

constexpr uint32_t kMagic = 0x4C504301;

struct CacheBlob {
    uint32_t magic;
    int32_t latE6;
    int32_t lonE6;
    int16_t tempTenthsC;
    int16_t highTenthsC;
    int16_t lowTenthsC;
    int16_t weatherCode;
    int16_t sunriseMin;
    int16_t sunsetMin;
    uint8_t hasWeather;
    uint8_t hasPlace;
    uint32_t updatedUnix;
    char query[40];
    char place[32];
    char tzPosix[64];
};

CacheBlob gCache;
volatile bool gRefresh = false;
uint32_t gNextTryMs = 0;

const char* conditionFor(int code) {
    if (code == 0) return "Clear";
    if (code == 1) return "Mostly clear";
    if (code == 2) return "Partly cloudy";
    if (code == 3) return "Cloudy";
    if (code == 45 || code == 48) return "Fog";
    if (code >= 51 && code <= 57) return "Drizzle";
    if (code >= 61 && code <= 67) return "Rain";
    if (code >= 71 && code <= 77) return "Snow";
    if (code >= 80 && code <= 82) return "Showers";
    if (code >= 85 && code <= 86) return "Snow";
    if (code >= 95) return "Thunderstorm";
    return "Weather";
}

WeatherKind kindFor(int code) {
    if (code == 0 || code == 1) return WEATHER_CLEAR;
    if (code == 2) return WEATHER_PARTLY;
    if (code == 3) return WEATHER_CLOUDY;
    if (code == 45 || code == 48) return WEATHER_FOG;
    if ((code >= 71 && code <= 77) || code == 85 || code == 86) return WEATHER_SNOW;
    if (code >= 95) return WEATHER_STORM;
    if (code >= 51 && code <= 82) return WEATHER_RAIN;
    return WEATHER_UNKNOWN;
}

int parseClock(const char* iso) {
    if (!iso) {
        return -1;
    }
    const char* t = strchr(iso, 'T');
    if (!t || strlen(t) < 6) {
        return -1;
    }
    int hour = (t[1] - '0') * 10 + (t[2] - '0');
    int minute = (t[4] - '0') * 10 + (t[5] - '0');
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        return -1;
    }
    return hour * 60 + minute;
}

void applyToState(bool fromCache) {
    StateGuard guard;
    AppState& state = guard.get();
    if (!gCache.hasPlace) {
        return;
    }
    strncpy(state.place, gCache.place, sizeof(state.place) - 1);
    state.place[sizeof(state.place) - 1] = '\0';
    if (gCache.tzPosix[0] == '\0' && settings().tzPosix[0]) {
        strncpy(gCache.tzPosix, settings().tzPosix, sizeof(gCache.tzPosix) - 1);
    }
    if (!gCache.hasWeather) {
        state.weatherValid = false;
        return;
    }
    state.tempTenthsC = gCache.tempTenthsC;
    state.highTenthsC = gCache.highTenthsC;
    state.lowTenthsC = gCache.lowTenthsC;
    state.sunriseMin = gCache.sunriseMin;
    state.sunsetMin = gCache.sunsetMin;
    state.weather = kindFor(gCache.weatherCode);
    strncpy(state.condition, conditionFor(gCache.weatherCode), sizeof(state.condition) - 1);
    state.condition[sizeof(state.condition) - 1] = '\0';
    state.weatherValid = true;
    state.weatherFromCache = fromCache;
    state.note[0] = '\0';
}

void saveCache() {
    Preferences prefs;
    if (!prefs.begin("lpc", false)) {
        return;
    }
    prefs.putBytes("cache", &gCache, sizeof(gCache));
    prefs.end();
}

void note(const char* text) {
    StateGuard guard;
    strncpy(guard.get().note, text, sizeof(guard.get().note) - 1);
    guard.get().note[sizeof(guard.get().note) - 1] = '\0';
}

bool ianaToPosix(const char* iana, char* out, size_t n) {
    struct Row { const char* iana; const char* posix; };
    static const Row rows[] = {
        {"America/New_York", "EST5EDT,M3.2.0,M11.1.0"},
        {"America/Toronto", "EST5EDT,M3.2.0,M11.1.0"},
        {"America/Detroit", "EST5EDT,M3.2.0,M11.1.0"},
        {"America/Chicago", "CST6CDT,M3.2.0,M11.1.0"},
        {"America/Winnipeg", "CST6CDT,M3.2.0,M11.1.0"},
        {"America/Denver", "MST7MDT,M3.2.0,M11.1.0"},
        {"America/Edmonton", "MST7MDT,M3.2.0,M11.1.0"},
        {"America/Phoenix", "MST7"},
        {"America/Los_Angeles", "PST8PDT,M3.2.0,M11.1.0"},
        {"America/Vancouver", "PST8PDT,M3.2.0,M11.1.0"},
        {"America/Halifax", "AST4ADT,M3.2.0,M11.1.0"},
        {"Europe/London", "GMT0BST,M3.5.0/1,M10.5.0"},
        {"Europe/Paris", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Europe/Berlin", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Europe/Amsterdam", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Europe/Rome", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Europe/Madrid", "CET-1CEST,M3.5.0,M10.5.0"},
        {"Asia/Kolkata", "IST-5:30"},
        {"Asia/Calcutta", "IST-5:30"},
        {"Asia/Tokyo", "JST-9"},
        {"Asia/Dubai", "GST-4"},
        {"Asia/Singapore", "SGT-8"},
        {"Australia/Sydney", "AEST-10AEDT,M10.1.0,M4.1.0/3"},
        {"Pacific/Auckland", "NZST-12NZDT,M9.5.0,M4.1.0/3"},
    };
    for (const Row& row : rows) {
        if (strcmp(iana, row.iana) == 0) {
            strncpy(out, row.posix, n - 1);
            out[n - 1] = '\0';
            return true;
        }
    }
    return false;
}

void urlEncode(const char* in, char* out, size_t cap) {
    size_t j = 0;
    for (size_t i = 0; in[i] && j + 4 < cap; i++) {
        unsigned char c = (unsigned char)in[i];
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.') {
            out[j++] = (char)c;
        } else if (c == ' ') {
            out[j++] = '%';
            out[j++] = '2';
            out[j++] = '0';
        } else {
            snprintf(out + j, 4, "%%%02X", c);
            j += 3;
        }
    }
    out[j] = '\0';
}

bool httpsGet(const char* url, String& body) {
    static WiFiClientSecure secure;
    static bool ready = false;
    if (!ready) {
        secure.setInsecure();
        ready = true;
    }
    HTTPClient http;
    http.setTimeout(8000);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    if (!http.begin(secure, url)) {
        return false;
    }
    int code = http.GET();
    if (code != 200) {
        logf("weather: request returned %d", code);
        http.end();
        return false;
    }
    body = http.getString();
    http.end();
    return body.length() > 2;
}

bool geocode() {
    char encoded[120];
    char url[220];
    urlEncode(settings().city, encoded, sizeof(encoded));
    snprintf(url, sizeof(url),
             "https://geocoding-api.open-meteo.com/v1/search?name=%s&count=1&language=en&format=json",
             encoded);
    String body;
    if (!httpsGet(url, body)) {
        note("Could not look up that city.");
        return false;
    }
    JsonDocument doc;
    if (deserializeJson(doc, body)) {
        note("Could not read the city reply.");
        return false;
    }
    JsonObject result = doc["results"][0];
    if (result.isNull()) {
        note("I couldn't find that city.");
        logf("weather: no match for %s", settings().city);
        return false;
    }
    gCache.latE6 = (int32_t)(result["latitude"].as<float>() * 1000000.0f);
    gCache.lonE6 = (int32_t)(result["longitude"].as<float>() * 1000000.0f);
    const char* name = result["name"] | settings().city;
    strncpy(gCache.place, name, sizeof(gCache.place) - 1);
    gCache.place[sizeof(gCache.place) - 1] = '\0';
    strncpy(gCache.query, settings().city, sizeof(gCache.query) - 1);
    gCache.query[sizeof(gCache.query) - 1] = '\0';
    gCache.hasPlace = 1;
    const char* iana = result["timezone"] | "";
    if (!ianaToPosix(iana, gCache.tzPosix, sizeof(gCache.tzPosix))) {
        gCache.tzPosix[0] = '\0';
        logf("weather: timezone %s will use the clock setting", iana);
    } else if (settings().tzPosix[0] == '\0') {
        logf("weather: clock timezone from %s", iana);
    }
    gCache.magic = kMagic;
    saveCache();
    logf("weather: %s", gCache.place);
    return true;
}

bool fetchForecast() {
    if (!gCache.hasPlace) {
        return false;
    }
    char url[320];
    snprintf(url, sizeof(url),
             "https://api.open-meteo.com/v1/forecast?latitude=%ld.%06ld&longitude=%ld.%06ld"
             "&current=temperature_2m,weather_code"
             "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset"
             "&timezone=auto&forecast_days=1&temperature_unit=celsius",
             (long)(gCache.latE6 / 1000000), (long)labs(gCache.latE6 % 1000000),
             (long)(gCache.lonE6 / 1000000), (long)labs(gCache.lonE6 % 1000000));
    String body;
    if (!httpsGet(url, body)) {
        note("Weather will try again soon.");
        return false;
    }
    JsonDocument doc;
    if (deserializeJson(doc, body)) {
        note("Weather reply was unreadable.");
        return false;
    }
    if (doc["current"]["temperature_2m"].isNull()) {
        note("Weather reply was incomplete.");
        return false;
    }
    float temp = doc["current"]["temperature_2m"].as<float>();
    int code = doc["current"]["weather_code"].as<int>();
    float high = doc["daily"]["temperature_2m_max"][0] | temp;
    float low = doc["daily"]["temperature_2m_min"][0] | temp;
    gCache.tempTenthsC = (int16_t)(temp * 10.0f);
    gCache.highTenthsC = (int16_t)(high * 10.0f);
    gCache.lowTenthsC = (int16_t)(low * 10.0f);
    gCache.weatherCode = (int16_t)code;
    gCache.sunriseMin = (int16_t)parseClock(doc["daily"]["sunrise"][0]);
    gCache.sunsetMin = (int16_t)parseClock(doc["daily"]["sunset"][0]);
    gCache.hasWeather = 1;
    gCache.updatedUnix = (uint32_t)time(nullptr);
    gCache.magic = kMagic;
    if (gCache.tzPosix[0] == '\0') {
        int offset = doc["utc_offset_seconds"] | 0;
        int hours = offset / 3600;
        int mins = abs(offset % 3600) / 60;
        snprintf(gCache.tzPosix, sizeof(gCache.tzPosix), "UTC%+d:%02d", -hours, mins);
    }
    saveCache();
    applyToState(false);
    logf("weather: %s, %d.%d C", conditionFor(code),
         gCache.tempTenthsC / 10, abs(gCache.tempTenthsC % 10));
    return true;
}

bool due(uint32_t nowMs) {
    return (int32_t)(nowMs - gNextTryMs) >= 0;
}

}  // namespace

void weatherLoad() {
    memset(&gCache, 0, sizeof(gCache));
    gCache.sunriseMin = -1;
    gCache.sunsetMin = -1;
    Preferences prefs;
    if (prefs.begin("lpc", true)) {
        CacheBlob stored;
        size_t n = prefs.getBytes("cache", &stored, sizeof(stored));
        prefs.end();
        if (n == sizeof(stored) && stored.magic == kMagic) {
            gCache = stored;
        }
    }
    if (settings().city[0] && gCache.hasPlace && strcmp(gCache.query, settings().city) == 0) {
        applyToState(true);
        logf("weather: saved sky for %s", gCache.place);
    } else {
        gCache.hasPlace = 0;
        gCache.hasWeather = 0;
    }
}

void weatherRequestSoon() {
    gRefresh = true;
}

const char* weatherSavedTz() {
    if (settings().tzPosix[0]) {
        return settings().tzPosix;
    }
    return gCache.tzPosix;
}

void weatherMaintain(uint32_t nowMs) {
    if (!settings().city[0]) {
        note("Add your city in secrets.h");
        return;
    }
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }
    bool cityChanged = !gCache.hasPlace || strcmp(gCache.query, settings().city) != 0;
    if (cityChanged) {
        if (!due(nowMs) && !gRefresh) {
            return;
        }
        gRefresh = false;
        gNextTryMs = nowMs + LPC_WEATHER_RETRY_MS;
        note("Looking up your city...");
        if (!geocode()) {
            return;
        }
    }
    bool stale = !gCache.hasWeather || gCache.updatedUnix < 1700000000 ||
                 (time(nullptr) > (time_t)gCache.updatedUnix &&
                  time(nullptr) - (time_t)gCache.updatedUnix > 20 * 60);
    if (!stale && !gRefresh) {
        return;
    }
    if (!due(nowMs) && !gRefresh) {
        return;
    }
    if (gRefresh && gCache.updatedUnix > 1700000000 &&
        time(nullptr) - (time_t)gCache.updatedUnix < 60) {
        gRefresh = false;
        return;
    }
    gRefresh = false;
    note("Asking the sky...");
    if (fetchForecast()) {
        gNextTryMs = nowMs + LPC_WEATHER_OK_MS;
    } else {
        gNextTryMs = nowMs + LPC_WEATHER_RETRY_MS;
        if (gCache.hasWeather) {
            applyToState(true);
        }
    }
}
