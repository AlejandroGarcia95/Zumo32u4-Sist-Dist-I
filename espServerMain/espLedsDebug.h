#pragma once

#include <ESP8266WiFi.h>

#define LEDS_DELAY 50

#define LED_GREEN 5
#define LED_RED 13

// Must be called inside setup
void setupLedsDebug();

void showLedsDebug(bool success);
