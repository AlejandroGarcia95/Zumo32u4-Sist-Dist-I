#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"

/* Global IDs for our protocols */
int selfID = 2;
String selfIDString = String("2");

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

// ---------------------------- MAIN PROGRAM ----------------------------

// Called only once when robot starts
void setup() {
  setupZumoMovement();
  Serial.begin(9600);
  setupToEsp();
  setupProximity();
  setupLedsDebug();
  lastBtnTime = millis();
  ledYellow(0);
  showLedsDebug(true);
}


/* Function states for LOST robots */

/*
 * The robot in idle state will keep idling with it's sensors on.
 * If it detects something, it will send a message to the leader
 * and await for a ACK and a order to start a handshake.
 */
void idle() {
  Serial.println("Entering IDLE state\n");
  while(!objectIsInFront()) {
    delay(1000);
  }
  // Found something, send message and turn led to indicate!
  String msg = "";
  msg = createMessage(MSG_ICU, selfIDString);
  sendToEsp(msg);
  ledYellow(1);

  // Now wait for response
  wait_hs();
}

/*
 * The robot freezes waiting for a message from the Leader, to
 * start a new connection. If that message takes too much time
 * to arrive (or is never sent) the robot will re-enter in idle 
 * mode. Alternativelly, if the message is a rejection to start
 * handshake (e.g. because two lost robots have detected the leader 
 * at the same time) the robot will also re-enter idle state 
 * after a small delay.
 */
void wait_hs() {
  Serial.println("Entering WAIT_HS state\n");
  // Expect message from leader to start handshake
  String msg = "";
  msg = receiveFromEsp(true);
  Serial.println(msg);

  // Idicate that a message have been read with leds and re-enter idle state
  ledYellow(0);
  ledGreen(1);
  delay(2000);
  ledGreen(0);
  idle();
}


/* Function states for LEADER robots */

/*
 * A Leader in searching state starts moving forwards (TODO: 
 * make it turn) and constantly hears for messages from any
 * Lost robot that may have seen it.
 */
void searching() {
  String msg = "";
  bool found = false;
  while(!found) {
    delay(2000);
    moveDistanceInTime(20, 3, false);

    delay(5000);
    // Expect message from lost to start handshake
    msg = receiveFromEsp(false);
    Serial.println(msg);
    String msgType = getMessageType(msg); 
    if(msgType == MSG_ICU) {
      Serial.println("Match!\n");
      found = true; 
    }
  }

  // Answer message (Payload of msg contains Lost robot ID)
  msg = createMessage(MSG_DEBUG, String("Leader says hi!"));
  sendToEsp(msg);
  
  // Enter idle mode
  idle();
}


// Press A to become lost, B to become Leader
void loop() {
  while(true) {
    delay(1000);
    if(buttonA.isPressed()) {
      // I'm idle!
      idle();
    } else if (buttonB.isPressed()) {
      searching();  
    }
  }
}


// Main loop
void loop_old() {
  // Send message to ESP if a button was pressed
  String msg = "";
  if((millis() - lastBtnTime) > 2000) {
    if(buttonA.isPressed()) {
      msg = createMessage(MSG_MOVE, String("5"));
      sendToEsp(msg);
      lastBtnTime = millis();
      Serial.println(msg);
    }
    else if(buttonB.isPressed()) {
      msg = createMessage(MSG_MOVE, String("-5"));
      sendToEsp(msg);
      lastBtnTime = millis();
      Serial.println(msg);
    }
    else if(buttonC.isPressed()) {
      msg = createMessage(MSG_ROTATE, String("45"));
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
