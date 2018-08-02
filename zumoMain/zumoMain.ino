#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoMovement.h"
#include "zumoProximity.h"
#include "zumoLedsDebug.h"
#include "zumoToEsp.h"


// Special topics used for our protocols

const String LDR_ELECTION_TOPIC = "election";
const String LOST_TOPIC = "lost";
const String LEADER_TOPIC = "leader";

// Global IDs for our protocols

const int robotsAmount = 4; // Including leader
const int myRobotId = 0;
const String ROBOT_NAMES[] = {"Cassandra", "Maria", "Mongo", "Neo"};

// Various constants for the protocols

#define DRAIN_ATTEMPTS 20 // Used by drainMode
#define SOURCE_TRANSMIT_DELAY 40 // Used by sourceMode
#define DELTA_PHI 89 // Used for UFMP
#define LDR_ELECTION_TIMEOUT 10000 // Used for leader election

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
  bool imSeen = false;
  while(!imSeen){
    
    while(getMessageType(msg) != MSG_ICU) {
      delay(30);
      msg = receiveFromEsp();
    }
    // Check if I can see the leader too
    SAY("Leader saw something, checking if it was me...");
    if(drainMode(DELTA_PHI)){
        msg = createMessage(MSG_CU2, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
        sendToEsp(msg);
        // TODO: should wait for leader response
        SAY("Hey, you found me!");
        imSeen = true;
      }
    else {
      msg = createMessage(MSG_CUN, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
      sendToEsp(msg);
      SAY("Nope, it was not me!");
    }
    
  }

  // TODO: Change for a fancier exit
  UNSUBSCRIBE_FROM(LOST_TOPIC);
  ledYellow(1);
  delay(500);
  rotate(47, true);
  rotate(47, true);
  moveDistanceInTime(28, 3, false);
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
  int robotsFound = 0;
  while(robotsFound < (robotsAmount - 1)) {
    // Move until something is found
    spiralWalk();
    
    // Found something, tell all lost robots
    SAY("I see something");
    String msg = createMessage(MSG_ICU, "", LOST_TOPIC);
    sendToEsp(msg);
    ledYellow(1);
    
    // Wait for lost robots to reply
    int robotsReplies = 0;
    bool foundSomeone = false;
    while(robotsReplies < (robotsAmount - robotsFound - 1)){ 
      msg = sourceMode();
      if(getMessageType(msg) == MSG_CU2) {
        delay(1000);
        SAY("I found " + getMessagePayload(msg) + "!");
        robotsReplies++;
        robotsFound++;
        // TODO: Send response to robot
        foundSomeone = true;
      }
      else if(getMessageType(msg) == MSG_CUN) {
        delay(1000);
        SAY("I haven't found " + getMessagePayload(msg) + " as they can't see me");
        robotsReplies++;
      }
    }
  
    if(foundSomeone) {
      delay(2600); // Allow the found robot to step aside
      if((robotsAmount - robotsFound) > 1)
        SAY("I still need to find " + String(robotsAmount - robotsFound - 1) + " lost robots");
    } else
      SAY("What I found was not a robot fella");
  
  }

  // TODO: Do something funny
  SAY("I finally found all my robot mates");
  ledYellow(0);
  ledGreen(1);
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

// TODO: Move inside the searching function
int spiral_steps = 1;
int spiral_stage = 0;
int spiral_act = 0;

void spiralWalk(){
  long rnd;
  bool found = false;
  while(!found) {
    delay(1000);
    if (spiral_act < spiral_steps) {
      moveDistanceInTime(8, 3, false);
      spiral_act++;
    } else {
      // rotate
      spiral_act = 0;
      rotate(48, true);
      rotate(47, true);
      if (spiral_stage % 2 == 1) {
        spiral_steps++;
      }
      spiral_stage++;
    }
    
    if(objectIsInFront()) {
      found = true;
    }
  }
}

// ---------------------- AUXILIAR FUNCTIONS ----------------------

// Shouldn't be needed, but just in case
void subscribeToSelfTopic(){
  SUBSCRIBE_TO(ROBOT_ID_TO_NAME(myRobotId));
}

void waitForEsp(){
  Serial.println("\nWaiting for ESP to be ready...");
  String msg = receiveFromEsp();
  while(getMessageType(msg) != MSG_ERDY) {
    //delay(30);
    msg = receiveFromEsp();
  }
  Serial.println("ESP is ready!");
}

void leaderElection(){
  /*
  if(myRobotId == 0) {// Cassandra is the leader
    imLeader = true;
    SUBSCRIBE_TO(LEADER_TOPIC);
    SAY("I am the leader");
  }
  */
  SAY("I'm voting in the leader election");
  int leaderId = myRobotId;
  SUBSCRIBE_TO(LDR_ELECTION_TOPIC);
  // Publish own id as leader
  String msg = createMessage(MSG_LDRE, String(leaderId), LDR_ELECTION_TOPIC);
  sendToEsp(msg);
  
  // Wait a while checking if a better leader shows up
  unsigned long startingTime = millis();
  while((millis() - startingTime) < LDR_ELECTION_TIMEOUT){
    msg = receiveFromEsp();
    if(getMessageType(msg) == MSG_NONE)
      delay(30);
    else if(getMessageType(msg) == MSG_LDRE){
      int newId = getMessagePayload(msg).toInt();
      if(newId <= leaderId) // There's a better leader
        leaderId = newId;
      else { // I Know a better leader
        msg = createMessage(MSG_LDRE, String(leaderId), LDR_ELECTION_TOPIC);
        sendToEsp(msg);
      }     
    }
    
  }

  UNSUBSCRIBE_FROM(LDR_ELECTION_TOPIC);
  if(leaderId == myRobotId) {
    imLeader = true;
    SAY("I am the leader");
    SUBSCRIBE_TO(LEADER_TOPIC);
    // Give lost robots some time to enter lost state
    delay(LDR_ELECTION_TIMEOUT / 3);
  }
  else
    SAY("All hail the leader " + ROBOT_ID_TO_NAME(leaderId));

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
  randomSeed(analogRead(0));
  setupZumoMovement();
  Serial.begin(9600);
  setupToEsp();
  setupProximity();
  setupLedsDebug();
  waitForEsp();
  subscribeToSelfTopic();
  ledYellow(0);
  ledRed(1);
  SAY("I have connected");
  ledGreen(1);
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


