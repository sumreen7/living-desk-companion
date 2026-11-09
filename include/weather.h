#pragma once

#include <stdint.h>

void weatherLoad();
void weatherMaintain(uint32_t nowMs);
void weatherRequestSoon();
const char* weatherSavedTz();
