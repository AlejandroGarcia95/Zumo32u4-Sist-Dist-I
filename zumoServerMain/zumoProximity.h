#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

/* This two constants are used in the objectIsInFront function for
detecting a near object. Depending on their value, the distance used
for considering an object as "near" will vary. Some values measured:
SENSOR_THRESHOLD: 4, SENSOR_PERIOD: 480, "near distance" is: 24 cm;
SENSOR_THRESHOLD: 5, SENSOR_PERIOD: 470, "near distance" is: 20 cm;
SENSOR_THRESHOLD: 5, SENSOR_PERIOD: 480, "near distance" is: 15 cm */
#define SENSOR_THRESHOLD 1
#define SENSOR_PERIOD 420

// These objects must be defined in your sketch.
extern Zumo32U4ProximitySensors proxSensors;


// Must be called inside setup
void setupProximity();

/* Returns true if an object is found in front of the robot. The value of
SENSOR_THRESHOLD is used to decide whether an object is near enough.*/
bool objectIsInFront();


void detectWithoutEmiting();
