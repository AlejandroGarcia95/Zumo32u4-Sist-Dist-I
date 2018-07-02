#pragma once

#include <Wire.h>
#include <Zumo32U4.h>


#define SENSOR_PERIOD 421
#define SENSOR_DUTY 408

#define TRANSMITIONS_AMOUNT 6
#define TRANSMITION_THRESHOLD 1
#define TRANSMITION_DELAY 1

#define DETECTIONS_AMOUNT 7
#define DETECTION_THRESHOLD 3
#define DETECTION_DELAY 0

// These objects must be defined in your sketch.
extern Zumo32U4ProximitySensors proxSensors;


// Must be called inside setup
void setupProximity();

/* Returns true if an object is found in front of the robot.*/
bool objectIsInFront();

void transmitIRPulses();

bool detectIRPulses();

