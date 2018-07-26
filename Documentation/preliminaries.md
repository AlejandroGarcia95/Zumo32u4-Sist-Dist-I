# Preliminaries

## 1. Purpose


## 2. The Zumo 32U4 robot

​	The main ingredient of our project is Pololu's Zumo 32U4, a mini-sumo preassembled robot controlled by an Arduino-compatible ATmega32U4. Among some useful things, the Zumo 32U4 has a pair of motors with encoders, proximity IR sensors, a gyro and a micro-USB port. You can find the Zumo 32U4 user's guide [here](https://www.pololu.com/docs/0J63/all). Also, since the robot has its own easy to use library, you can find [many examples on Github](https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples). You may also want to check  the [library documentation](http://pololu.github.io/zumo-32u4-arduino-library/).

### 2.1. Zumo 32U4 pinout

​	In order to communicate the robots between each other, we first needed to release some of the ATmega32U4 pins of the Zumo for use. We achieved  this by removing the LCD display of the robot as shown in the picture below (do not worry: the LC can be easily unplugged and plugged back again later).

![](Zumo32U4_pinout.jpg)

​	Once removed, the LCD releases many pins of the Zumo robot microcontroller. Four of those pins were needed, as marked on the image below: a GND and 5v pair of pins, which will serve as a voltage source for the ESP module (as will be explained on the latest section of this document); and pins 0 and 1 from the microcontroller, known respectively as RXD1 and TXD1. These two RX and TX pins control one of the UART serial modules of the ATmega32U4 microcontroller, hence allowing us to send and receive data to the ESP (again, explained on the latest section).	

![](Zumo32U4_pinout.jpg)

### 2.2. Programming the Zumo 32U4 

​	For programming the Zumo 32U4 robot on your Arduino IDE, first you will have to install the Zumo board and drivers [by following these simple instructions](https://github.com/pololu/a-star), and then install the Zumo32U4 library with the IDE's Library Manager. If you don't know how to do this last thing, just [follow this steps](https://www.pololu.com/docs/0J63/6) from the Zumo robot's documentation. 

​	After everything has been set, we will be able to program the Zumo robot with the Arduino IDE by plugging it into an USB port. If you quickly want to test your robot, just follow the next steps for programming it with one of the examples provided with the library:

- Click on the "Tools" menu, select "Board" and then check "Pololu A-Star 32U4".
- Under the "File" menu, select "Examples" > "Zumo32U4" and click any example you would like to program. If this is the first time you are programming your Zumo, you should go with the "BlinkLEDs" example. The example .ino file should open on your Arduino IDE after choosing it.
- Select the USB port your Zumo 32U4 is connected to under "Tools" > "Port".
- Finally, compile and upload your code to the Zumo robot by clicking "Sketch" > "Upload" or the upload arrow button on the IDE button bar.

## 3. The ESP8285 NodeMCU

​	The wireless communication between robots in our project is achieved by using NodeMCU modules based on the ESP8285. This microchip has Wi-Fi capabilities with a full TCP/IP stack, being able to perform as both a TCP server or client. You can find the ESP8285 datasheet [here](https://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf) and many examples [on Github](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples). In our case, we used the ESP8285 M2 NodeMCU board of DOIT, [available at Amazon](https://www.amazon.com/Makerfocus-ESP8285-Development-Compatible-ESP8266/dp/B075K48R1Q).

​	**Disclaimer**: Although we used ESP8285 NodeMCU modules, _any_ ESP module should work just fine for this project. As long as the same electrical connections are used, the code should work as fine as with the NodeMCU.

### 3.1. NodeMCU pinout

​	The picture below shows the needed pins from the NodeMCU for our project. Similarly to what happened with the Zumo 32U4, we need a pair of pins for GND and 5v (to power up the module), and the RX and TX pins for communication. In this case, these two pins use the same UART module as the USB port of the board. Note the TX on pin 2 for Serial1 **cannot** be used, since it has no RX pin associated.

![](ESP8285_pinout.jpg)

​	**Disclaimer**: If the NodeMCU module used is not the DOIT board, pinout would likely differ. As long as pin connections are kept the same, everything should work fine.

### 3.2. Programming the ESP8285 NodeMCU

​	Before programming the NodeMcu, you must first install the ESP board and library [following this guide](https://github.com/esp8266/Arduino/blob/master/doc/installing.rst). Once you are ready, plug your NodeMCU on some USB port and follow the next steps:

- Click on the "Tools" menu, select "Board" and then check "Generic ESP8285 Module". If you are using any other ESP module, you will have to check that particular board (and some of the next options may not be available).
- Under the "File" menu, select "Examples" > "ESP8266" > "Blink". The blinking LED example should open. Click on "Tools" > "Builtin Led" > "2" and connect an LED between GPIO 2 and GND.
- Click on "Tools" > "Reset method" > "nodemcu".
- Click on "Tools" > "Flash Size" > "1M (64K SPIFFS)".
- Select the USB port your NodeMCU is connected to under "Tools" > "Port".
- Finally, compile and upload your code to the NodeMCU by clicking "Sketch" > "Upload" or the upload arrow button on the IDE button bar. The LED you previously connected should start blinking.

### 3.3. Using MQTT on the NodeMCU

## 4. Communicating the Zumo and NodeMCU

​	Now that is clear how the Zumo 32U4 and the Node MCU operate, we need to electrically connect them to allow interaction with the MQTT broker on the Zumo robot. Since the NodeMCU cannot be directly plugged into the Zumo robot like the LCD display removed, we needed to make a PCB board for every robot like the one showed in the images below: 

![](EspToZumoCircuit.jpg)

​	The connections used for the PCB are detailed below. Basically, as explained when analizing the pinout of both NodeMCU and Zumo robot, we used the RX/TX UART pair for communication (RX of one device connected to TX of the other, granting data exchange), and the GND and 5v pins from the robot to power on the NodeMCU. We additionally added two LEDs on GPIOs 5 and 13 of the NodeMCU (green and red, respectively), both optional, and used only for debugging purposes (the red LED is blinked if an error occurs, and the green one if everything was successful). 

![](EspToZumoCircuit.jpg)

​	**Disclaimer**: If another ESP module is being used, you may want to add some voltage regulating circuit on the PCB for granting 3.3v to power it up instead of the 5v of the Zumo robot, [like this one](https://iot-playground.com/blog/2-uncategorised/17-esp8266-wifi-module-and-5v-arduino-connection).

​	This way, we can send and receive data from both Zumo 32U4 and ESP8285 NodeMCU using their respective UART modules. The many functions of [Arduino Serial](https://www.arduino.cc/reference/en/language/functions/communication/serial/) help us making this communication rather easy: by simply printing a string on Zumo Serial1, we can retrieve it on the NodeMCU with a reading from its Serial. On the other hand, we can send data from the NodeMCU to the Zumo robot by printing on the NodeMCU Serial and reading from the Zumo Serial1. 

​	Hence, a message sent from one robot to another follows the path of the image below. As seen, the message is first sent from the origin robot to its own NodeMCU (the one it is connected to), using their respective UART modules. The NodeMCU, which is a client for MQTT, publishes that message on some topic related to the second robot (destination robot on the picture). The MQTT broker handles that message, and forwards it to the NodeMCU on the destination robot, as long as it was a client previously subscribed to the topic of that message. The flow finishes with the NodeMCU sending the message via UART once more to the destination robot, which can finally process it.

![](EspToZumoCircuit.jpg)