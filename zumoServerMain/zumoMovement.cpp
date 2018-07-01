#include <Wire.h>
#include <Zumo32U4.h>

#include "turnSensor.h"
#include "zumoLedsDebug.h"
#include "zumoMovement.h"

// Must be called inside setup
void setupZumoMovement(){
  turnSensorSetup();
  delay(660);
  turnSensorReset();
}

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
  if(distCm < 1){
    showLedsDebug(false);
    return;
  }
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
  showLedsDebug(true);
}

/* Rotates the robot angleDeg degrees. If rotClockwise is true, then the
rotation is done in that direction, else the opposite (i.e. to the left).
The rotation is made with a constant speed TURN_SPEED.
IMPORTANT: angleDeg must be at most 90. */
void rotate(int angleDeg, bool rotClockwise) {
  if((angleDeg > 90) || (angleDeg < 1)){
    showLedsDebug(false);
    return;
  }
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
  showLedsDebug(true);
}

/* Makes the robot push the target in front away. Basically it moves the
robot forward BODY_SLAM_DIST cm in BODY_SLAM_TIME seconds, and then 
backwards. Note the backwards speed is a little slower because yes.*/
void bodySlam(){
  moveDistanceInTime(BODY_SLAM_DIST, BODY_SLAM_TIME, false);
  moveDistanceInTime(BODY_SLAM_DIST, BODY_SLAM_TIME + 1, true);
}
