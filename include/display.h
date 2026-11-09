#pragma once

#include "canvas.h"

bool displayPanelReady();
bool displayStart();
bool displayMakeSprite();
void displayColorTest();
void displayPushFrame();
Canvas displayCanvas(int ox);
lgfx::LGFX_Device& displayDevice();
