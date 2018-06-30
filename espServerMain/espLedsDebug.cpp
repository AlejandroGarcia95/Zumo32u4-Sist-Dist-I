#include <ESP8266WiFi.h>

#include "espLedsDebug.h"


// Must be called inside setup
void setupLedsDebug() {
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_RED, OUTPUT);
	digitalWrite(LED_GREEN, 0);
	digitalWrite(LED_RED, 0);
}

void showLedsDebug(bool success) {
	if(success) {
		digitalWrite(LED_GREEN, 1);
		delay(LEDS_DELAY);
		digitalWrite(LED_GREEN, 0);
	} else {
		digitalWrite(LED_RED, 1);
		delay(LEDS_DELAY);
		digitalWrite(LED_RED, 0);
	}
}
