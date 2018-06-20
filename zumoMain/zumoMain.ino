#include <Wire.h>
#include <Zumo32U4.h>

#include "turnsensor.h"

/* Global variables that magically map themselves with the robot's
true hardware stuff (think of them as singleton objects). */

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Encoders encoders;
Zumo32U4ProximitySensors proxSensors;
L3G gyro;

/* --------------------------------------------------------------------
Experimentally measured constants. May wanna touch them if you
don't feel comfortable with your robot performance. 
----------------------------------------------------------------------*/

/* Constants for the moveDistanceInTime function. Basically they convert
motor speed and encoders counting into time and distance.*/
#define TIME_SEC_TO_SPEED(timeSec) (638 / timeSec)
#define DIST_CM_TO_COUNT(distCm) (78 * distCm)
#define MIN_SPEED 64

// Motor speed for rotating
#define TURN_SPEED 85

/* This two constants are used in the objectIsInFront function for
detecting a near object. Depending on their value, the distance used
for considering an object as "near" will vary. Some values measured:
SENSOR_THRESHOLD: 4, SENSOR_PERIOD: 480, "near distance" is: 24 cm;
SENSOR_THRESHOLD: 5, SENSOR_PERIOD: 470, "near distance" is: 20 cm;
SENSOR_THRESHOLD: 5, SENSOR_PERIOD: 480, "near distance" is: 15 cm */
#define SENSOR_THRESHOLD 5
#define SENSOR_PERIOD 480

/* Constants for the bodySlam function. They define the distance the
robot move forward (pushing the target) and the time spent for that
(i.e. the speed used for pushing the target away). Note they should
be correlated with the "near distance" defined above.*/
#define BODY_SLAM_DIST 20 
#define BODY_SLAM_TIME 3 // Change to 1 for high speed hit


// -------------------------- AUXILIAR FUNCTIONS --------------------------

/* Moves the robot forward a distance of distCm centimeters, in (around)
timeSec seconds. If reverseDir is true, then the robot moves backwards.
This function uses three constants, all experimentally measured. Distance
measures seem pretty well, although time measures seem to be really tricky
(floor conditions affect motors' speed, and there's also a MIN_SPEED
constraint since the robot doesn't move at all with little speeds). Feel
free to modify TIME_SEC_TO_SPEED if you don't like it that much.

If you wanna have an idea of how fast the robot will move, here you have:
< 10 cm/s : TURTLE MODE; 20 cm/s : SLOW; 30 cm/s : NORMAL; 50 cm/s : FAST;
> 80 cm/s : ROBOT USED QUICK ATTACK. */
void moveDistanceInTime(int distCm, int timeSec, bool reverseDir){
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();

  // motorSpeed must never be less than MIN_SPEED
  uint16_t motorSpeed = max(MIN_SPEED, TIME_SEC_TO_SPEED(timeSec));
  long count = DIST_CM_TO_COUNT(distCm);
  if(reverseDir)
      motorSpeed = -motorSpeed;

  long countsLeft = 0;
  long countsRight = 0;

  motors.setSpeeds(motorSpeed, motorSpeed);
  while(min(countsLeft, countsRight) < count) {
    if(reverseDir) {
      countsLeft -= encoders.getCountsAndResetLeft();
      countsRight -= encoders.getCountsAndResetRight();
    }
    else {
      countsLeft += encoders.getCountsAndResetLeft();
      countsRight += encoders.getCountsAndResetRight();
    }

    delay(2);
  };

  motors.setSpeeds(0, 0);
}

/* Rotates the robot angleDeg degrees. If rotClockwise is true, then the
rotation is done in that direction, else the opposite (i.e. to the left).
The rotation is made with a constant speed TURN_SPEED.
IMPORTANT: angleDeg must be at most 90. */
void rotate(int angleDeg, bool rotClockwise) {
  turnSensorReset();
  if(rotClockwise)
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  else
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if(rotClockwise)
      angle = -angle;
  } while(angle < angleDeg);
  
  motors.setSpeeds(0, 0);
}

/* Returns true if an object is found in front of the robot. The value of
SENSOR_THRESHOLD is used to decide whether an object is near enough.*/
bool objectIsInFront() {
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();
  
  return ((leftValue >= SENSOR_THRESHOLD) || (rightValue >= SENSOR_THRESHOLD));  
}

/* Makes the robot push the target in front away. Basically it moves the
robot forward BODY_SLAM_DIST cm in BODY_SLAM_TIME seconds, and then 
backwards. Note the backwards speed is a little slower because yes.*/
void bodySlam(){
  moveDistanceInTime(BODY_SLAM_DIST, BODY_SLAM_TIME, false);
  moveDistanceInTime(BODY_SLAM_DIST, BODY_SLAM_TIME + 1, true);
}


// ---------------------------- MAIN PROGRAM ----------------------------

// Called only once when robot starts
void setup() {
  turnSensorSetup();
  delay(500);
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(0, INPUT_PULLUP);
  Serial1.flush();
  Serial1.readString();
  turnSensorReset();
  ledGreen(0);
  proxSensors.initFrontSensor();
  proxSensors.setPeriod(SENSOR_PERIOD);
  Serial.println("On!");
}

// Main loop
void loop() {
  String inStr = "";
  inStr = Serial1.readString();
  Serial.print(inStr);
  int angle = inStr.toInt();
  if(angle > 1)
    rotate(angle, false);
}
