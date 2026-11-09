#pragma once

#include <time.h>

bool clockRead(struct tm& out);
int clockMinuteOfDay(const struct tm& t);
void clockFormatTime(char* buf, size_t n, const struct tm& t, bool withPeriod);
void clockFormatDate(char* buf, size_t n, const struct tm& t);
