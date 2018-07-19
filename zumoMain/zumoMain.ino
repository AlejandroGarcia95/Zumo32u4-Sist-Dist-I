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

// Global IDs for our protocols */
const int myRobotId = 1;
const String ROBOT_NAMES[] = {"Mongo", "Cassandra", "Maria", "Neo"};

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


// Shouldn't be needed, but just in case
void subscribeToSelfTopic(){
  String msg = createMessage(MSG_SUB, "", ROBOT_ID_TO_NAME(myRobotId));
  sendToEsp(msg);
}

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

  while(true) {
    delay(30);
    String msg = receiveFromEsp();
    Serial.println(msg);
    if(getMessageType(msg) == MSG_ICU) {
      break;
    }
  }

  SAY("Hey, you found me!");
  ledYellow(1);
  UNSUBSCRIBE_FROM(LOST_TOPIC);
}


/* Function states for LEADER robots */

/*
 * A Leader in searching state starts moving forwards (TODO: 
 * make it turn) and constantly hears for messages from any
 * Lost robot that may have seen it.
 */
void searching() {
  SAY("I am looking for robots");
  
  bool found = false;
  while(!found) {
    delay(1000);
    moveDistanceInTime(5, 3, false);

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


