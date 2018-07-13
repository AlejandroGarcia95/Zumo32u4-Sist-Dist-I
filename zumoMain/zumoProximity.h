#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

// These objects must be defined in your sketch.
extern Zumo32U4ProximitySensors proxSensors;

/* --------------------------------------------------------------------
Experimentally measured constants. May wanna touch them if you
don't feel comfortable with your robot's performance. 
----------------------------------------------------------------------*/

/* Defines the IR pulses' period and duty. Since the Zumo's clock is
16MHz, a SENSOR_PERIOD constant 0f 420 (recommended value) defines a
period of 420/16MHz = 26.25Us (around 38kHz frequency). */
#define SENSOR_PERIOD 421
#define SENSOR_DUTY 408

/* Constants for the objectIsInFront function. Basically they define the
amount of consecutive IR transmissions, the delay between them, and how
many of them should be successful for considering an object as "near".*/
#define TRANSMISSIONS_AMOUNT 6
#define TRANSMISSION_THRESHOLD 1
#define TRANSMISSION_DELAY 1

/* Constants for the detectIRPulses function. Basically they define the
amount of consecutive sensor readings, the delay between them, and how
many of them should be successful for considering an IR emitter as "near".*/
#define DETECTIONS_AMOUNT 7
#define DETECTION_THRESHOLD 3
#define DETECTION_DELAY 0

/* --------------------------------------------------------------------
      FUNCTIONS
----------------------------------------------------------------------*/

// Must be called inside setup
void setupProximity();

/* Returns true if an object is found in front of the robot. This function
emits IR pulses and checks if they have bounced back. TRANSMISSIONS_AMOUNT
IR pulses are emited, with a delay of TRANSMISSION_DELAY miliseconds between
each one. This function returns true if at least TRANSMISSION_THRESHOLD of 
them bounced back to the proximity sensors.*/
bool objectIsInFront();

/* Behaves exactly the same as objectIsInFront, except that does not return
true or false. You should call this function only if you want to turn on the
IR emiters. May wanna deprecate this on the near future.*/
void transmitIRPulses();

/* Returns true if IR pulses are reaching the front sensors. This function
only senses IR pulses WITHOUT emitting them previously. DETECTIONS_AMOUNT
readings of the sensors are performed, with a delay of DETECTION_DELAY 
miliseconds between each one. This function returns true if at least
DETECTION_THRESHOLD of the readings detected IR pulses.*/
bool detectIRPulses();

