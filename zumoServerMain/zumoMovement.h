#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

// These objects must be defined in your sketch.
extern Zumo32U4Motors motors;
extern Zumo32U4Encoders encoders;
extern L3G gyro;

/* --------------------------------------------------------------------
Experimentally measured constants. May wanna touch them if you
don't feel comfortable with your robot's performance. 
----------------------------------------------------------------------*/

/* Constants for the moveDistanceInTime function. Basically they convert
motor speed and encoders counting into time and distance.*/
#define TIME_SEC_TO_SPEED(timeSec) (638 / timeSec)
#define DIST_CM_TO_COUNT(distCm) (78 * distCm)
#define MIN_SPEED 64

// Motor speed for rotating
#define TURN_SPEED 85

/* Constants for the bodySlam function. They define the distance the
robot move forward (pushing the target) and the time spent for that
(i.e. the speed used for pushing the target away). Note they should
be correlated with the "near distance" defined above.*/
#define BODY_SLAM_DIST 20 
#define BODY_SLAM_TIME 3 // Change to 1 for high speed hit

/* --------------------------------------------------------------------
			FUNCTIONS
----------------------------------------------------------------------*/

// Must be called inside setup
void setupZumoMovement();

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
void moveDistanceInTime(int distCm, int timeSec, bool reverseDir);

/* Rotates the robot angleDeg degrees. If rotClockwise is true, then the
rotation is done in that direction, else the opposite (i.e. to the left).
The rotation is made with a constant speed TURN_SPEED.
IMPORTANT: angleDeg must be at most 90. */
void rotate(int angleDeg, bool rotClockwise);

/* Makes the robot push the target in front away. Basically it moves the
robot forward BODY_SLAM_DIST cm in BODY_SLAM_TIME seconds, and then 
backwards.*/
void bodySlam();



