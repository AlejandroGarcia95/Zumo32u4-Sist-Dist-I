#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"

/* Global variables that magically map themselves with the robot's
true hardware stuff (think of them as singleton objects). */

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
L3G gyro;

// ---------------------------- MAIN PROGRAM ----------------------------

// Called only once when robot starts
void setup() {
  setupZumoMovement();
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(0, INPUT_PULLUP);
  Serial1.flush();
  Serial1.readString();
  setupProximity();
  setupLedsDebug();
  ledYellow(0);
}

/*
// Main loop
void loop() {
  String commandStr = "";
  if(Serial1.available() > 2) {
    
    commandStr = Serial1.readString();
    Serial.print(commandStr);
    if(commandStr.startsWith("R")) {
      // Rotate
      int angle = commandStr.substring(1).toInt();
      Serial.println(angle);
      if(angle > 1)
        rotate(angle, false);
      else if(angle < -1)
        rotate(-angle, true);
      else
        showLedsDebug(false);
    }
    else if(commandStr.startsWith("M")) {
      // Move
      int dist = commandStr.substring(1).toInt();
      if(dist > 1)
        moveDistanceInTime(dist, 3, false);
      else if(dist < -1)
        moveDistanceInTime(-dist,3, true);
      else
        showLedsDebug(false);
    }
    else {
      showLedsDebug(false);
    }
    
  }
}
*/
void loop() {
  if(buttonB.isPressed())
    detectWithoutEmiting();
  if(buttonA.isPressed()) {
    while(1) {
      if(objectIsInFront())
        ledYellow(1);
      else
        ledYellow(0);
    }
  }
}

