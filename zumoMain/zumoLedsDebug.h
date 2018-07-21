#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

#define LEDS_DELAY 50

// Must be called inside setup
void setupLedsDebug();

// Blinks the green or red led of the Zumo
// (green if success is true, red if false).
void showLedsDebug(bool success);

