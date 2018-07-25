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

const int myRobotId = 0;
const String ROBOT_NAMES[] = {"Mongo", "Cassandra", "Maria", "Neo"};

// Various constants for the protocols

#define DRAIN_ATTEMPTS 20 // Used by drainMode
#define SOURCE_TRANSMIT_DELAY 40 // Used by sourceMode
#define HALF_HANDSHAKE_ITERATIONS 3 // Used by handshake
#define CLOCKWISE true
#define ANTICLOCKWISE false
#define UNTIL_SEE_SOMETHING true
#define UNTIL_SEE_NOTHING false

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
  if(drainMode(10, true, true)){
    msg = createMessage(MSG_CU2, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
    sendToEsp(msg);

    // TODO: should wait for leader response
    SAY("Hey, you found me!");

    // Initiate handshake
    bool aligned = handshake(false, LEADER_TOPIC, 16);
    if (aligned) {
      ledYellow(1);
      UNSUBSCRIBE_FROM(LOST_TOPIC);
      SAY("We are aligned!!!");
    }
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
  String foundOne;

  while(true){ // TODO: loop until all robots reply (add counter)
    msg = sourceMode();
    if(getMessageType(msg) == MSG_CU2) {
      foundOne = getMessagePayload(msg);
      SAY("I found " + foundOne + "!");
      break;
    }
    if(getMessageType(msg) == MSG_CUN) {
      SAY("I found an obstacle");
      break;
    }
  }

  delay(2000);
  SAY("I'm gonna start aligning, dude!");
  // TODO: keep only one robot, reject others even if they CU2
  // Starting alignment 
  bool aligned = handshake(true, foundOne, 16);
  if (aligned) {
      ledYellow(0);
      ledGreen(1);
      SAY("We are aligned!!!");
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

  // TODO: change to a for in order to avoid infinite loop
  // if no response
  while(getMessageType(msg) == MSG_NONE){
    objectIsInFront();
    delay(SOURCE_TRANSMIT_DELAY);
    msg = receiveFromEsp();
  }

  return msg;
}

/* Turns the robot into drain mode, passively sensing IR without emiting
pulses. This function makes the robot rotate delta_phi degrees clockwise
(or anti-clockwise, depending on the second parameter), before
performing DRAIN_ATTEMPTS attempts of IR pulses detection.
If any of these attempts does not match the third parameter, the function
returns true WITH THE ROBOT STANDING IN THAT POSITION (i.e. the robot
will be facing towards the direction where it first sensed pulses, or stoped
sensing them). If no changes in IR pulses detection where made after having
rotated 360 degrees, the function returns -1, else it returns the angle
turned.*/
int drainMode(int delta_phi, bool isClockwise, bool something) {
  for (int i = 0; i <= 360; i += delta_phi) {
    rotate(delta_phi, isClockwise);

    bool foundPulses = false;
    for(int j = 0; j < DRAIN_ATTEMPTS; j++)
      if(detectIRPulses())
        foundPulses = true;
    
    if(something == foundPulses)
      return i;
  }

  return -1;
}

/* Perform a handshake to align two robots. The robots are identified by its
topics: this function will align self robot and robot listening at other_topic.
The varialbe first should be set in only one of the robots, to inidicate who is
going to be rotating first.
Robots perform an alignment based on bisecting the range in which they sense the
other robot, by moving increments of delta_phi.
The constant HALF_HANDSHAKE_ITERATIONS indicates how many "half handshakes" will be
performed: a half handshake involves one robot rotating while the other is sourcing.
After a half handshake, the robots switch places. A full handshake step ends when
the two robots have gone through both roles. If there are half handshakes left, another
iteration will begin, but the increments delta_phi will be halved as an attempt to
inprove accuracy.

If an error occurs at any time (e.g. a message of the wrong type is received), 
the function abruptly ends by returning false. Otherwise, it returns true when
both robots have finished alignment.
Powered by: modulo 2 arithmetic
*/
bool handshake(bool first, String other_topic, int delta_phi) {
  SAY("On handshake against " + other_topic);
  String msg;
  int stage = (first ? 1 : 0);
  SAY("Starting handshake on stage " + String(stage));

  for (int i = 0; i < HALF_HANDSHAKE_ITERATIONS; i++) {
    if((stage % 2) == 1) {
      delay(200);
      SAY("I am sourcing on half handshake " + String(i));
      msg = createMessage(MSG_SA, "", other_topic);
      sendToEsp(msg);
      
      // Start sourcing and wait for response
      msg = sourceMode();
      if (getMessageType(msg) != MSG_FA){
        SAY("Unexpected message type while alignment");
        return false;
      }

    } else {
      SAY("I am rotating on half handshake " + String(i));
      // Wait for signal to start rotating
      msg = receiveFromEsp();
      while(getMessageType(msg) == MSG_NONE){
        delay(20);
        msg = receiveFromEsp();
      }
      if (getMessageType(msg) != MSG_SA){
        SAY("Unexpected message type while alignment");
        return false;
      }

      // I must rotate
      int turned = drainMode(delta_phi, ANTICLOCKWISE, UNTIL_SEE_NOTHING);
      if (turned < 0) return false;

      turned = drainMode(delta_phi, CLOCKWISE, UNTIL_SEE_SOMETHING);
      if (turned < 0) return false;

      turned = drainMode(delta_phi, CLOCKWISE, UNTIL_SEE_NOTHING);
      if (turned < 0) return false;

      // Anticlockwise half the ammount turned last time
      rotate(turned * 0.5, ANTICLOCKWISE);

      SAY("I'm alligned");
      msg = createMessage(MSG_FA, "", other_topic);
      sendToEsp(msg);
    }
    stage++;

    // If a whole handshake step has ended, halve the angle
    if ((i % 2) == 1) {
      delta_phi /= 2;
    }
  }

  SAY("Finishing handshake");
  return true;
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
  buttonA.waitForButton();
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


