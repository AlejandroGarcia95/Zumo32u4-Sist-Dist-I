#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoLedsDebug.h"
#include "zumoProximity.h"


// Must be called inside setup
void setupProximity() {
  proxSensors.initFrontSensor();
  proxSensors.setPeriod(SENSOR_PERIOD);
  uint16_t brightLvl[] = {SENSOR_DUTY};
  proxSensors.setBrightnessLevels(brightLvl, 1);
  delay(100);
}

/* Returns true if an object is found in front of the robot.*/
bool objectIsInFront() {
  uint8_t successfulReadings = 0;
  for(int i = 0; i < TRANSMITIONS_AMOUNT; i++) {
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t rightValue = proxSensors.countsFrontWithRightLeds();
    
    if(leftValue || rightValue)
      successfulReadings++;

    delay(TRANSMITION_DELAY);
  }

  return (successfulReadings >= TRANSMITION_THRESHOLD);
}

void transmitIRPulses(){
  objectIsInFront();
}

bool detectIRPulses(){
  uint8_t successfulReadings = 0;
  for(int i = 0; i < DETECTIONS_AMOUNT; i++) {
    proxSensors.pullupsOn();
    proxSensors.lineSensorEmittersOff();
    
    if(proxSensors.readBasicFront())
      successfulReadings++;

    delay(DETECTION_DELAY);
  }

  return (successfulReadings >= DETECTION_THRESHOLD);  
}



