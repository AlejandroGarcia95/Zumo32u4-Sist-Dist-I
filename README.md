# Zumo32u4-Sist-Dist-I

## Uploading .ino to ESP8285 Node MCU

- Select board as "Generic 8285 module"
- Select flash size as "1M (64K SPIFFS)"
- Select reset method "nodemcu"

## Connection diagram

Connection for this example (no need to have Pololu or ESP connected to PC via USB):

|  POLOLU   | ESP  |
| :-------: | :--: |
|    GND    | GND  |
|   RXD1    |  Tx  |
|   TXD1    |  Rx  |
| 5v output | Vin  |

Also, ESP pin 5 blinks an LED when request is completed.

Usage: go to URI ESP_IP:8080/rotate?angle=X     with 1 < X < 90 and ESP_IP the IP of the ESP. Remember to edit the Wifi and password net on the .ino file!



## The Zumo 32U4 robot

​	The main ingredient of our project is Pololu's Zumo 32U4, a mini-sumo preassembled robot controlled by an Arduino-compatible ATmega32U4. Among some useful things, the Zumo 32U4 has a pair of motors with encoders, proximity IR sensors, a gyro and a micro-USB port. You can find the Zumo 32U4 user's guide [here](https://www.pololu.com/docs/0J63/all). Also, since the robot has its own easy to use library, you can find [many examples on Github](https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples). You may also want to check  the [library documentation](http://pololu.github.io/zumo-32u4-arduino-library/).

