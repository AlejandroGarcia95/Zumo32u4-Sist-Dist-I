#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoLedsDebug.h"
#include "zumoProximity.h"


// Must be called inside setup
void setupProximity() {
  proxSensors.initFrontSensor();
  proxSensors.setPeriod(SENSOR_PERIOD);
  uint16_t brightLvl[] = {SENSOR_PERIOD - 8};
  proxSensors.setBrightnessLevels(brightLvl, 1);
  delay(100);
}

/* Returns true if an object is found in front of the robot. The value of
SENSOR_THRESHOLD is used to decide whether an object is near enough.*/
bool objectIsInFront() {
  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();

  Serial.print("L: ");
  Serial.println(leftValue);
  Serial.print("R: ");
  Serial.println(rightValue);
  
  
  return ((leftValue >= SENSOR_THRESHOLD) || (rightValue >= SENSOR_THRESHOLD));  
}



void detectWithoutEmiting(){
  while(true){
    proxSensors.pullupsOn();
    proxSensors.lineSensorEmittersOff();
    bool p = proxSensors.readBasicFront();
    if(p)
      ledYellow(1);
    else
      ledYellow(0);
  }
}



