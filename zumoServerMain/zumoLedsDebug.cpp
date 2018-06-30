#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoLedsDebug.h"


// Must be called inside setup
void setupLedsDebug() {
	ledGreen(0);
	ledRed(0);
}

void showLedsDebug(bool success) {
	if(success) {
		ledGreen(1);
		delay(LEDS_DELAY);
		ledGreen(0);
	} else {
		ledRed(1);
		delay(LEDS_DELAY);
		ledRed(0);
	}
}
