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

/* Returns true if an object is found in front of the robot. This function
emits IR pulses and checks if they have bounced back. TRANSMISSIONS_AMOUNT
IR pulses are emited, with a delay of TRANSMISSION_DELAY miliseconds between
each one. This function returns true if at least TRANSMISSION_THRESHOLD of 
them bounced back to the proximity sensors.*/
bool objectIsInFront() {
  uint8_t successfulReadings = 0;
  for(int i = 0; i < TRANSMISSIONS_AMOUNT; i++) {
    proxSensors.read();
    uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
    uint8_t rightValue = proxSensors.countsFrontWithRightLeds();
    
    if(leftValue || rightValue)
      successfulReadings++;

    delay(TRANSMISSION_DELAY);
  }

  return (successfulReadings >= TRANSMISSION_THRESHOLD);
}

/* Behaces exactly the same as objectIsInFront, except that does not return
true or false. You should call this function only if you want to turn on the
IR emiters. May wanna deprecate this on the near future.*/
void transmitIRPulses(){
  objectIsInFront();
}

/* Returns true if IR pulses are reaching the front sensors. This function
only senses IR pulses WITHOUT emitting them previously. DETECTIONS_AMOUNT
readings of the sensors are performed, with a delay of DETECTION_DELAY 
miliseconds between each one. This function returns true if at least
DETECTION_THRESHOLD of the readings detected IR pulses.*/
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



