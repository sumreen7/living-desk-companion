#include "messages.h"

static uint8_t gNudge = 0;

void messageAdvance() {
    gNudge++;
}

static const char* pick(const char* const* list, int count, uint32_t nowMs) {
    uint32_t index = nowMs / 180000UL + gNudge;
    return list[index % (uint32_t)count];
}

const char* messageFor(SkyPeriod period, uint32_t nowMs) {
    static const char* kMorning[] = {
        "Good morning.",
        "One thing at a time.",
        "You've got this.",
        "Start softly.",
    };
    static const char* kDay[] = {
        "The sky looks pretty.",
        "Small steps count.",
        "Take a little break.",
        "You're doing great.",
    };
    static const char* kEvening[] = {
        "It's okay to slow down.",
        "Look at the sky.",
        "You can soften now.",
    };
    static const char* kNight[] = {
        "Rest your eyes.",
        "You did enough today.",
        "The night can be quiet.",
        "Tomorrow can wait.",
    };

    switch (period) {
        case SKY_EARLY_MORNING:
        case SKY_SUNRISE:
        case SKY_MORNING:
            return pick(kMorning, 4, nowMs);
        case SKY_GOLDEN_HOUR:
        case SKY_SUNSET:
        case SKY_DUSK:
            return pick(kEvening, 3, nowMs);
        case SKY_NIGHT:
            return pick(kNight, 4, nowMs);
        case SKY_AFTERNOON:
        default:
            return pick(kDay, 4, nowMs);
    }
}
