#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"

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
  setupToEsp();
  setupProximity();
  setupLedsDebug();
  ledYellow(0);
}


// Main loop
void loop() {
  String msg = "";
  msg = receiveFromEsp(true);
  Serial.print(msg);
  String msgType = getMessageType(msg);
  Serial.print(msgType);
  if(msgType == MSG_ROTATE) {
    // Rotate
    int angle = getMessagePayload(msg).toInt();
    Serial.println(angle);
    if(angle > 1)
      rotate(angle, false);
    else if(angle < -1)
      rotate(-angle, true);
    else
      showLedsDebug(false);
  }
  else if(msgType == MSG_MOVE) {
    // Move
    int dist = getMessagePayload(msg).toInt();
    Serial.println(dist);
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


/*
void loop() {
  if(buttonB.isPressed()) {
    while(1) {
      if(detectIRPulses())
        ledYellow(1);
      else
        ledYellow(0);
    }
  }
  if(buttonA.isPressed()) {
    while(1) {
      delay(50);
      if(objectIsInFront())
        ledYellow(1);
      else
        ledYellow(0);
    }
  }
}
*/
