#include "clock.h"

#include <stdio.h>
#include <string.h>

#include "settings.h"

bool clockRead(struct tm& out) {
    time_t now = time(nullptr);
    if (now < 1700000000) {
        return false;
    }
    localtime_r(&now, &out);
    return out.tm_year + 1900 >= 2024;
}

int clockMinuteOfDay(const struct tm& t) {
    return t.tm_hour * 60 + t.tm_min;
}

void clockFormatTime(char* buf, size_t n, const struct tm& t, bool withPeriod) {
    if (!settings().clock12h) {
        snprintf(buf, n, "%02d:%02d", t.tm_hour, t.tm_min);
        return;
    }
    int hour = t.tm_hour % 12;
    if (hour == 0) {
        hour = 12;
    }
    if (withPeriod) {
        snprintf(buf, n, "%d:%02d %s", hour, t.tm_min, t.tm_hour >= 12 ? "PM" : "AM");
    } else {
        snprintf(buf, n, "%d:%02d", hour, t.tm_min);
    }
}

void clockFormatDate(char* buf, size_t n, const struct tm& t) {
    static const char* kWeek[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char* kMonth[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    int w = t.tm_wday;
    int m = t.tm_mon;
    if (w < 0 || w > 6) {
        w = 0;
    }
    if (m < 0 || m > 11) {
        m = 0;
    }
    snprintf(buf, n, "%s %s %d", kWeek[w], kMonth[m], t.tm_mday);
}
