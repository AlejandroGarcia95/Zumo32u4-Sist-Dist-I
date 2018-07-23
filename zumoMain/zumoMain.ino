#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"


// Special topics used for our protocols

const String LOST_TOPIC = "lost";
const String FOLLOWERS_TOPIC = "followers";
const String LEADER_TOPIC = "leader";

// Global IDs for our protocols

const int myRobotId = 1;
const String ROBOT_NAMES[] = {"Mongo", "Cassandra", "Maria", "Neo"};

// Various constants for the protocols

#define DRAIN_ATTEMPTS 20 // Used by drainMode
#define SOURCE_TRANSMIT_DELAY 40 // Used by sourceMode

// Fancy macros

#define ROBOT_ID_TO_NAME(id) (ROBOT_NAMES[id])
#define SAY(msg) sendDebugMessage(ROBOT_ID_TO_NAME(myRobotId) + ": " + msg)

#define SUBSCRIBE_TO(topic) (sendToEsp(createMessage(MSG_SUB, "", topic)))
#define UNSUBSCRIBE_FROM(topic) (sendToEsp(createMessage(MSG_UNSUB, "", topic)))

/* Global variables that magically map themselves with the robot's
true hardware stuff (think of them as singleton objects). */

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
L3G gyro;


bool imLeader = false;

/* Function states for LOST robots */

/*
 * The robot in idle state will keep idling with it's sensors on.
 * If it detects something, it will send a message to the leader
 * and await for a ACK and a order to start a handshake.
 */
void idle() {
  SUBSCRIBE_TO(LOST_TOPIC);
  SAY("Entering IDLE state");
  String msg = receiveFromEsp();;

  // Loop until the leader sees me
  while(getMessageType(msg) != MSG_ICU) {
    delay(30);
    msg = receiveFromEsp();
  }
  // Check if I can see the leader too
  SAY("Leader saw something, checking if it was me...");
  if(drainMode(10)){
      msg = createMessage(MSG_CU2, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
      sendToEsp(msg);
      // TODO: should wait for leader response
      SAY("Hey, you found me!");
      ledYellow(1);
      UNSUBSCRIBE_FROM(LOST_TOPIC);
    }
  else {
    msg = createMessage(MSG_CUN, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
    sendToEsp(msg);
    SAY("Nope, it was not me!");
  }
}

/* Function states for LEADER robots */

/*
 * A Leader in searching state starts moving forwards (TODO: 
 * make it turn) and constantly senses if there is something
 * in front. If there is something, it sends a message to the
 * lost robots and enters state leader_source()
 */
void searching() {
  SAY("I am looking for robots");
  // Move until something is found
  bool found = false;
  while(!found) {
    delay(1000);
    moveDistanceInTime(5, 3, false);

    if(objectIsInFront()) {
      found = true;
    }  
  }
  
  // Found something, tell all lost robots
  SAY("I see something");
  String msg = createMessage(MSG_ICU, "", LOST_TOPIC);
  sendToEsp(msg);
  ledYellow(1);
  
  // Wait for lost robots to reply
  while(true){ // TODO: loop until all robots reply (add counter)
    msg = sourceMode();
    if(getMessageType(msg) == MSG_CU2) {
      SAY("I found someone!");
      ledYellow(0);
      ledGreen(1);
      break;
    }
    if(getMessageType(msg) == MSG_CUN) {
      SAY("I found an obstacle");
      break;
    }
  }

}

// ------------------------ UFMP FUNCTIONS ------------------------

/* Turns the robot into source mode, making it emit IR pulses with a
delay of SOURCE_TRANSMIT_DELAY between each consecutive transmition.
This function will make the robot emit pulses until a message arrives
from the ESP, returning it as a String. */
String sourceMode() {
  SAY("Turning LEDs on and waiting for response...");
  String msg = receiveFromEsp();

  while(getMessageType(msg) == MSG_NONE){
    objectIsInFront();
    delay(SOURCE_TRANSMIT_DELAY);
    msg = receiveFromEsp();
  }

  return msg;
}

/* Turns the robot into drain mode, passively sensing IR without emiting
pulses. This function makes the robot rotate delta_phi degrees clockwise
before performing DRAIN_ATTEMPTS attempts of IR pulses detection. If any
of these attempts detects IR pulses, the function returns true WITH THE
ROBOT STANDING IN THAT POSITION (i.e. the robot will be facing towards 
the IR pulses' source). If no IR pulses were ever detected after having
rotated 360 degrees, the function returns false.*/
bool drainMode(int delta_phi){
  for (int i = 0; i <= 360; i += delta_phi) {
    rotate(delta_phi, true);
    
    for(int i = 0; i < DRAIN_ATTEMPTS; i++)
      if(detectIRPulses())
        return true;
  }

  return false;
}

// ---------------------- AUXILIAR FUNCTIONS ----------------------

// Shouldn't be needed, but just in case
void subscribeToSelfTopic(){
  String msg = createMessage(MSG_SUB, "", ROBOT_ID_TO_NAME(myRobotId));
  sendToEsp(msg);
}

void waitForEsp(){
  Serial.println("\nWaiting for ESP to be ready...");
  String msg = receiveFromEsp();
  while(getMessageType(msg) != MSG_ERDY) {
    delay(30);
    msg = receiveFromEsp();
  }
  Serial.println("ESP is ready!");
}

void leaderElection(){
  // Nice to have: A real leader election
  delay(1000);
  if(myRobotId == 1) {// Cassandra is the leader
    imLeader = true;
    SUBSCRIBE_TO(LEADER_TOPIC);
    SAY("I am the leader");
  }
}

// ---------------------------- MAIN PROGRAM ----------------------------

void leaderMain(){
  searching();
  // TODO: Finish in a more decent manner
  while(true) delay(3000);
}

void followerMain(){
  idle();
  // TODO: Finish in a more decent manner
  while(true) delay(3000);
}

// Called only once when robot starts
void setup() {
  setupZumoMovement();
  Serial.begin(9600);
  setupToEsp();
  setupProximity();
  setupLedsDebug();
  waitForEsp();
  subscribeToSelfTopic();
  ledYellow(0);
  SAY("I have connected");
  showLedsDebug(true);
  leaderElection();
}


void loop() {
  // Pretty straight forward
  if(imLeader)
    leaderMain();
  else
    followerMain();
}


