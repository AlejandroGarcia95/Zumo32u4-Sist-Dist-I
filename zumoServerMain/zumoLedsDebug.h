#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

#define LEDS_DELAY 50

// Must be called inside setup
void setupLedsDebug();

void showLedsDebug(bool success);
