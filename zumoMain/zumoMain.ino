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

#define LDR_ELECTION_TIMEOUT 9000 // Used for leader election
#define DRAIN_ATTEMPTS 20 // Used by drainMode
#define SOURCE_TRANSMIT_DELAY 40 // Used by sourceMode
#define DELTA_PHI 12 // Used for UFMP
#define DODGING_ANGLE 30 // Used to dodge obstacles
#define SPIRAL_CELL 5 // Used in spiralWalk

// Fancy macros

#define ROBOT_ID_TO_NAME(id) (ROBOT_NAMES[id])
#define SAY(msg) sendDebugMessage(ROBOT_ID_TO_NAME(myRobotId) + ": " + msg)

#define SUBSCRIBE_TO(topic) (sendToEsp(createMessage(MSG_SUB, "", topic)))
#define UNSUBSCRIBE_FROM(topic) (sendToEsp(createMessage(MSG_UNSUB, "", topic)))

/* Global variables that magically map themselves with the robot's
  true hardware stuff (think of them as singleton objects). */

Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
L3G gyro;

bool imLeader = false;

// --------------- LEADER AND LOST MAIN FUNCTIONS ---------------

/* Main function for a lost robot. All lost robots will wait for
  the leader to see something, and then start the UFMP to check
  if they can see the leader too by sensing the IR pulses emitted
  from it. The proper answer will be forwarded to the leader, and
  the robot will "leave stage" after being found. */
void lostMain(){
  SUBSCRIBE_TO(LOST_TOPIC);
  SAY("Entering IDLE state");
  String msg = receiveFromEsp();;

  // Be a lost robot until the leader finds me
  bool imSeen = false;
  while(!imSeen){
    // Wait for the leader to see something
    while(getMessageType(msg) != MSG_ICU){
      delay(30);
      msg = receiveFromEsp();
    }
    // UFMP: check if I can see the leader too
    SAY("Leader saw something, checking if it was me...");
    if(drainMode(DELTA_PHI)){
      msg = createMessage(MSG_CU2, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
      sendToEsp(msg);
      SAY("Hey, you found me!");
      imSeen = true;
    }
    else{
      msg = createMessage(MSG_CUN, ROBOT_ID_TO_NAME(myRobotId), LEADER_TOPIC);
      sendToEsp(msg);
      SAY("Nope, it was not me!");
    }
  }

  // Exit from stage as I was already found
  UNSUBSCRIBE_FROM(LOST_TOPIC);
  ledYellow(1);
  ledRed(1);
  ledGreen(1);
  delay(500);
  moveDistanceInTime(60, 6, true);
  while(1)      // Nice to have: sleep mode or
    delay(3000);// do smth now that was found 
}

/* Main function for the leader robot. The leader's mission is
  to walk around trying to find the lost robots. Every time
  something is seen with the proximity sensors, the leader will
  start the UFMP by multicasting an ICU message to all lost robots.
  By collecting all lost robot's answers, the leader can know if
  it found a lost robot, or an obstacle (and will dodge it).
  This is repeated until all lost robots are found. */
void leaderMain(){
  SAY("I am looking for robots");
  int robotsFound = 0;
  // Continue while there are robots left
  while(robotsFound < (robotsAmount - 1)){
    // Move until something is found
    spiralWalk();

    // UFMP: Found something, tell all lost robots
    SAY("I see something");
    String msg = createMessage(MSG_ICU, "", LOST_TOPIC);
    sendToEsp(msg);

    // Wait for lost robots to reply
    int robotsReplies = 0;
    bool foundSomeone = false;
    while(robotsReplies < (robotsAmount - robotsFound - 1)){
      msg = sourceMode();
      if (getMessageType(msg) == MSG_CU2){
        delay(1000);
        SAY("I found " + getMessagePayload(msg) + "!");
        robotsReplies++;
        robotsFound++;

        foundSomeone = true;
      }
      else if(getMessageType(msg) == MSG_CUN){
        delay(1000);
        SAY("I haven't found " + getMessagePayload(msg) + " as they can't see me");
        robotsReplies++;
      }
    }

    // Allow the found robot to leave stage or dodge obstacle accordingly
    if(foundSomeone){
      delay(2600); // Allow the found robot to step aside
      if((robotsAmount - robotsFound) > 1)
        SAY("I still need to find " + String(robotsAmount - robotsFound - 1) + " lost robots");
    } else {
      SAY("What I found was not a robot fella");
      rotate(DODGING_ANGLE, true);
    }

  }

  // All robots were found
  SAY("I finally found all my robot mates");
  ledYellow(1);
  ledRed(1);
  ledGreen(1);
  while(1)      // Nice to have: sleep mode or
    delay(3000);// do smth now that was found
}

// ---------------------- AUXILIAR FUNCTIONS ----------------------

/* Turns the robot into source mode, making it emit IR pulses with a
  delay of SOURCE_TRANSMIT_DELAY between each consecutive transmition.
  This function will make the robot emit pulses until a message arrives
  from the ESP, returning it as a String. */
String sourceMode(){
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
  before performing DRAIN_ATTEMPTS attempts of IR pulses detection. If any
  of these attempts detects IR pulses, the function returns true WITH THE
  ROBOT STANDING IN THAT POSITION (i.e. the robot will be facing towards
  the IR pulses' source). If no IR pulses were ever detected after having
  rotated 360 degrees, the function returns false.*/
bool drainMode(int delta_phi){
  for(int i = 0; i <= 360; i += delta_phi){
    rotate(delta_phi, true);

    for(int i = 0; i < DRAIN_ATTEMPTS; i++)
      if (detectIRPulses())
        return true;
  }

  SAY("Finishing handshake");
  return true;
}

/* Walking function for the leader to cover the whole space in a spiral-shaped
  path. The spiral will be considered as "squared", like inside a grid, with
  every cell of size SPIRAL_CELL cm. This function makes the leader walk the
  squared spiral until something is detected with the proximity sensors. */
int spiral_steps = 1;
int spiral_stage = 0;
int spiral_act = 0;

void spiralWalk(){
  long rnd;
  bool found = false;
  while(!found){
    delay(1000);
    if (spiral_act < spiral_steps){
      moveDistanceInTime(SPIRAL_CELL, 2, false);
      spiral_act++;
    } else{
      // Rotating
      spiral_act = 0;
      rotate(48, true);
      rotate(47, true);
      if (spiral_stage % 2 == 1){
        spiral_steps++;
      }
      spiral_stage++;
    }

    if (objectIsInFront()){
      found = true;
    }
  }
}

/* Starting function for subscribing every robot to its own topic. */
void subscribeToSelfTopic(){
  SUBSCRIBE_TO(ROBOT_ID_TO_NAME(myRobotId));
}

/* Waits for the ESP to be ready for communication. Just makes the robot
  wait for the ERDY message from the ESP. */
void waitForEsp(){
  Serial.println("\nWaiting for ESP to be ready...");
  String msg = receiveFromEsp();
  while(getMessageType(msg) != MSG_ERDY){
    //delay(30);
    msg = receiveFromEsp();
  }
  Serial.println("ESP is ready!");
}

/* Initial leader election to determinate which robot will have to 
  look for the others. Basically, all robots publish their robotId
  to the LDR_ELECTION_TOPIC and the one with smaller robotId becomes
  the leader. Note the election will finish after LDR_ELECTION_TIMEOUT
  miliseconds after the last message received on LDR_ELECTION_TOPIC. */
void leaderElection(){
  SAY("I'm voting in the leader election");
  int leaderId = myRobotId;
  SUBSCRIBE_TO(LDR_ELECTION_TOPIC);
  // Publish own id as leader
  String msg = createMessage(MSG_LDRE, String(leaderId), LDR_ELECTION_TOPIC);
  sendToEsp(msg);

  // Wait a while checking if a better leader shows up
  unsigned long lastMsgTime = millis();
  while((millis() - lastMsgTime) < LDR_ELECTION_TIMEOUT){
    msg = receiveFromEsp();
    if(getMessageType(msg) == MSG_NONE)
      delay(30);
    else if(getMessageType(msg) == MSG_LDRE){
      lastMsgTime = millis();
      int newId = getMessagePayload(msg).toInt();
      if (newId <= leaderId) // There's a better leader
        leaderId = newId;
      else{ // I Know a better leader
        msg = createMessage(MSG_LDRE, String(leaderId), LDR_ELECTION_TOPIC);
        sendToEsp(msg);
      }
    }

  }

  UNSUBSCRIBE_FROM(LDR_ELECTION_TOPIC);
  if(leaderId == myRobotId){
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


// Called only once when robot starts
void setup(){
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


void loop(){
  // Pretty straight forward
  if(imLeader)
    leaderMain();
  else
    lostMain();
}


