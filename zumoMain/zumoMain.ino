#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"

/* Global IDs for our protocols */
#define HANDSHAKE_NWAYS 2

const int myRobotId = 1;
const String ROBOT_NAMES[] = {"Mongo", "Cassandra", "Maria", "Neo"};
const String LOST_TOPIC = "lost";

#define ROBOT_ID_TO_NAME(id) (ROBOT_NAMES[id])
#define SAY(msg) sendDebugMessage(ROBOT_ID_TO_NAME(myRobotId) + ": " + msg)

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
  String msg = createMessage(MSG_SUB, "", ROBOT_ID_TO_NAME(myRobotId));
  sendToEsp(msg);
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
  Serial.println("\nWaiting for button");
}


/* Function states for LOST robots */

/*
 * The robot in idle state will keep idling with it's sensors on.
 * If it detects something, it will send a message to the leader
 * and await for a ACK and a order to start a handshake.
 */
void idle() {
  String msg = createMessage(MSG_SUB, "", LOST_TOPIC);
  Serial.println(msg);
  sendToEsp(msg);
  SAY("Entering IDLE state");

  while(true) {
    delay(2000);
    msg = receiveFromEsp();
    Serial.println(msg);
    if(getMessageType(msg) == MSG_ICU) {
      break;
    }
  }

  SAY("Hey, you found me!");
  ledYellow(1);
}


/* Function states for LEADER robots */

/*
 * A Leader in searching state starts moving forwards (TODO: 
 * make it turn) and constantly hears for messages from any
 * Lost robot that may have seen it.
 */
void searching() {
  SAY("I am the leader");
  
  bool found = false;
  while(!found) {
    delay(1000);
    moveDistanceInTime(10, 3, false);

    if(objectIsInFront()) {
      found = true;
    }  
  }
  
  // Found something, send message and turn led to indicate
  SAY("I see something");
  String msg = createMessage(MSG_ICU, "", LOST_TOPIC);
  sendToEsp(msg);
  ledYellow(1);
}

/*
 * A robot in handshake rotate will start rotating  with its 
 * proximity sensors on but without emiting pulses. That is, 
 * it will only receive.
 * It has three stages: 1) rotate anti-cw until the sensors
 * sense notihng. 2) rotate cw until sensors sense something,
 * 3) start recording the angle and rotate cw until the sensors
 * sense nothing.
 * Take de measured angle alpha and rotate alpha/2 anti-cw.
 * 
 * The parameter indicates the round of the handshake. If it is
 * over the constant HANDSHAKE_NWAYS the robot will enter in state
 * follower() or adopt_follower().
 * 
 * Else the robot will enter in handsahke_still() state.
 */
void handshake_rotate(int nways) {


}

/*
 * A robot in handshake still will hold its position and turn its
 * LED emmiters on. However it will ignore the readings and instead
 * let the other robot do the work
 */
void handshake_still() {
  
}

void follower() {
  // TODO: fill this
  idle();
}

void adopt_follower() {
  // TODO: fill this
  idle();
}

// Press A to become lost, B to become Leader
void loop() {
  if(buttonA.isPressed()) {
    // I'm idle!
    idle();
  } else if (buttonB.isPressed()) {
    searching();
  }

}


