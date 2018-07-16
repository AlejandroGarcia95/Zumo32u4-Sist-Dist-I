#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"

const String MY_TOPIC = "Mongo";
const String OTHER_TOPIC = "Cassandra";

/* Global variables that magically map themselves with the robot's
true hardware stuff (think of them as singleton objects). */

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
L3G gyro;

unsigned long lastBtnTime;

void subscribeToSelfTopic(){
  String msg = createMessage(MSG_SUB, "", MY_TOPIC);
  sendToEsp(msg);
  Serial.println(msg);
}

// ---------------------------- MAIN PROGRAM ----------------------------

// Called only once when robot starts
void setup() {
  setupZumoMovement();
  Serial.begin(9600);
  setupToEsp();
  setupProximity();
  setupLedsDebug();
  subscribeToSelfTopic();
  ledYellow(0);
  lastBtnTime = millis();
  showLedsDebug(true);
}


// Main loop
void loop() {
  // Send message to ESP if a button was pressed
  String msg = "";
  if((millis() - lastBtnTime) > 2000) {
    if(buttonA.isPressed()) {
      msg = createMessage(MSG_MOVE, String("5"), OTHER_TOPIC);
      sendToEsp(msg);
      lastBtnTime = millis();
      Serial.println(msg);
    }
    else if(buttonB.isPressed()) {
      msg = createMessage(MSG_MOVE, String("-5"), OTHER_TOPIC);
      sendToEsp(msg);
      lastBtnTime = millis();
      Serial.println(msg);
    }
    else if(buttonC.isPressed()) {
      msg = createMessage(MSG_ROTATE, String("45"), OTHER_TOPIC);
      sendToEsp(msg);
      lastBtnTime = millis();
      Serial.println(msg);
    }

  }

  // Checks if a msg was sent from ESP and executes it
  msg = receiveFromEsp(false);
  //Serial.println(msg);
  String msgType = getMessageType(msg);
  if(msgType == MSG_NONE)
    return;  
  else if(msgType == MSG_ROTATE) {
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
