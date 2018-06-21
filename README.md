# Zumo32u4-Sist-Dist-I

## Uploading .ino to ESP8285 Node MCU

- Select board as "Generic 8285 module"
- Select flash size as "1M (64K SPIFFS)"
- Select reset method "node mcu"

## Connection diagram

Connection for this example (no need to have Pololu or ESP connected to PC via USB):

|  POLOLU   | ESP  |
| :-------: | :--: |
|    GND    | GND  |
|   RXD1    |  2   |
| 5v output | Vin  |

Also, ESP pin 5 blinks an LED when request is completed.

Usage: go to URI ESP_IP:8080/rotate?angle=X     with 1 < X < 90 and ESP_IP the IP of the ESP. Remember to edit the Wifi and password net on the .ino file!