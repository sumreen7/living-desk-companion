#pragma once

#include <stdint.h>

enum Page : uint8_t {
    PAGE_SKY = 0,
    PAGE_WEATHER,
    PAGE_MUSIC,
    PAGE_MOOD,
    PAGE_COUNT
};

enum SkyPeriod : uint8_t {
    SKY_EARLY_MORNING = 0,
    SKY_SUNRISE,
    SKY_MORNING,
    SKY_AFTERNOON,
    SKY_GOLDEN_HOUR,
    SKY_SUNSET,
    SKY_DUSK,
    SKY_NIGHT
};

enum WeatherKind : uint8_t {
    WEATHER_UNKNOWN = 0,
    WEATHER_CLEAR,
    WEATHER_PARTLY,
    WEATHER_CLOUDY,
    WEATHER_FOG,
    WEATHER_RAIN,
    WEATHER_SNOW,
    WEATHER_STORM
};

enum Mood : uint8_t {
    MOOD_STUDYING = 0,
    MOOD_WORKING,
    MOOD_RELAXING,
    MOOD_LISTENING,
    MOOD_BREAK,
    MOOD_SLEEPING,
    MOOD_COUNT
};

struct AppState {
    bool displayReady;
    bool wifiConnected;
    bool timeValid;
    bool weatherValid;
    bool weatherFromCache;
    bool musicPlaying;

    Page page;
    WeatherKind weather;
    Mood mood;

    int16_t tempTenthsC;
    int16_t highTenthsC;
    int16_t lowTenthsC;
    int16_t sunriseMin;
    int16_t sunsetMin;

    char condition[24];
    char place[32];
    char note[56];
};

struct StateGuard {
    StateGuard();
    ~StateGuard();
    AppState& get();
};

void appStateInit();
void stateSnapshot(AppState& out);
