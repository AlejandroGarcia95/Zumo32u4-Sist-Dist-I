# Zumo32u4-Sist-Dist-I
Connection for this example (no need to have Pololu or ESP connected to PC via USB):

|  POLOLU   | ESP  |
| :-------: | :--: |
|    GND    | GND  |
|   RXD1    |  2   |
| 5v output | Vin  |

Also, ESP pin 5 blinks an LED when request is completed.

Usage: go to URI ESP_IP:8080/rotate?angle=X     with 1 < X < 90 and ESP_IP the IP of the ESP. Remember to edit the Wifi and password net on the ino file!