# Wi-Fi based robot's finding over ESP8285 NodeMCU

## 1. Introduction

​​​​Fill me with some intro

## 2. Preliminaries

​​​​This preliminary section is intended to briefly present the tools and devices used in our project, highlighting some related guides, tutorials and examples we found useful while working with them. As such, we do not pursue to make a deep explanation on every topic, but a quick and simple exposition of the most relevant aspects for our project. You may then want to skip some of these sections if you are already familiar with the devices described, or already know how to perform the steps detailed on them.

**Note**: You will need to have Arduino IDE installed on your machine for programming the devices used in our project. You can download Arduino IDE [directly from their website](https://www.arduino.cc/en/Main/Software?).

### 2.1. The Zumo 32U4 robot

​​​​The main ingredient of our project is Pololu's Zumo 32U4, a mini-sumo preassembled robot controlled by an Arduino-compatible ATmega32U4. Among some useful things, the Zumo 32U4 has a pair of motors with encoders, proximity IR sensors, a gyro and a micro-USB port. You can find the Zumo 32U4 user's guide [here](https://www.pololu.com/docs/0J63/all). Also, since the robot has its own easy to use library, you can find [many examples on Github](https://github.com/pololu/zumo-32u4-arduino-library/tree/master/examples). You may also want to check  the [library documentation](http://pololu.github.io/zumo-32u4-arduino-library/).

#### 2.1.1. Zumo 32U4 pinout

In order to communicate the robots between each other, we first needed to release some of the ATmega32U4 pins of the Zumo for use. We achieved  this by removing the LCD display of the robot as shown in the picture below (do not worry: the LCD can be easily unplugged and plugged back again later).

![](LCD_remove.jpg)

Once removed, the LCD releases many pins of the robot's microcontroller. Four of those pins were needed, as marked on the image below: a GND and 5v pair of pins, which will serve as a voltage source for the ESP module (as will be explained on the latest section of this document); and pins 0 and 1 from the microcontroller, known respectively as RXD1 and TXD1. These two RX and TX pins control one of the UART serial modules of the ATmega32U4 microcontroller, hence allowing us to send and receive data to the ESP (again, explained on the latest section).

![](Zumo32U4_pinout.jpg)

#### 2.1.2. Programming the Zumo 32U4 

​​​​For programming the Zumo 32U4 robot on your Arduino IDE, first you will have to install the Zumo board and drivers [by following these simple instructions](https://github.com/pololu/a-star), and then install the Zumo32U4 library with the IDE's Library Manager. If you don't know how to do this last thing, just [follow this steps](https://www.pololu.com/docs/0J63/6) from the Zumo robot's documentation. 

​​​​After everything has been set, we will be able to program the Zumo robot with the Arduino IDE by plugging it into an USB port. If you quickly want to test your robot, just follow the next steps for programming it with one of the examples provided with the library:

- Click on the "Tools" menu, select "Board" and then check "Pololu A-Star 32U4".
- Under the "File" menu, select "Examples" > "Zumo32U4" and click any example you would like to program. If this is the first time you are programming your Zumo, you should go with the "BlinkLEDs" example. The example .ino file should open on your Arduino IDE after choosing it.
- Select the USB port your Zumo 32U4 is connected to under "Tools" > "Port".
- Finally, compile and upload your code to the Zumo robot by clicking "Sketch" > "Upload" or the upload arrow button on the IDE button bar.

### 2.2. The ESP8285 NodeMCU

​​​​The wireless communication between robots in our project is achieved by using NodeMCU modules based on the ESP8285. This microchip has Wi-Fi capabilities with a full TCP/IP stack, being able to perform as both a TCP server or client. You can find the ESP8285 datasheet [here](https://www.espressif.com/sites/default/files/0a-esp8285_datasheet_en_v1.0_20160422.pdf) and many examples [on Github](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi/examples). In our case, we used the ESP8285 M2 NodeMCU board of DOIT, [available at Amazon](https://www.amazon.com/Makerfocus-ESP8285-Development-Compatible-ESP8266/dp/B075K48R1Q).

**Note**: Although we used ESP8285 NodeMCU modules, _any_ ESP module should work just fine for this project. As long as the same electrical connections are used, the code should work as fine as with the NodeMCU.

#### 2.2.1. NodeMCU pinout

​​​​The picture below shows the needed pins from the NodeMCU for our project. Similarly to what happened with the Zumo 32U4, we need a pair of pins for GND and 5v (to power up the module), and the RX and TX pins for communication. In this case, these two pins use the same UART module as the USB port of the board. Note the TX on pin 2 for Serial1 **cannot** be used, since it has no RX pin associated.

![](ESP8285_pinout.jpg)

**Note**: If the NodeMCU module used is not the DOIT board, pinout would likely differ. As long as pin connections are kept the same, everything should work fine.

#### 2.2.2. Programming the ESP8285 NodeMCU

​​​​​Before programming the NodeMcu, you must first install the ESP board and library [following this guide](https://github.com/esp8266/Arduino/blob/master/doc/installing.rst). Once you are ready, plug your NodeMCU on some USB port and follow the next steps:

- Click on the "Tools" menu, select "Board" and then check "Generic ESP8285 Module". If you are using any other ESP module, you will have to check that particular board (and some of the next options may not be available).
- Under the "File" menu, select "Examples" > "ESP8266" > "Blink". The blinking LED example should open. Click on "Tools" > "Builtin Led" > "2" and connect an LED between GPIO 2 and GND.
- Click on "Tools" > "Reset method" > "nodemcu".
- Click on "Tools" > "Flash Size" > "1M (64K SPIFFS)".
- Select the USB port your NodeMCU is connected to under "Tools" > "Port".
- Finally, compile and upload your code to the NodeMCU by clicking "Sketch" > "Upload" or the upload arrow button on the IDE button bar. The LED you previously connected should start blinking.

#### 2.2.3. Using MQTT on the NodeMCU

​​​​Since we needed NodeMCUs to exchange messages between each other via Wi-Fi, we chose to use MQTT for that. If you are not familiar, MQTT is a [messaging protocol](https://internetofthingsagenda.techtarget.com/definition/MQTT-MQ-Telemetry-Transport) based on a publisher/subscriber system, which relies on a "broker node" for forwarding all messages to the proper client device. Basically, any client can subscribe to a topic (identified with a name) from which it wants to receive messages, and publish a message on any topic it wants. Every time a client publishes something, such message is sent to the broker, which will forward it to all other clients who have previously subscribed to that topic.

![](mqtt.jpg)

​​​​There are many MQTT implementations that can satisfy this need. For this project, we recommend [installing Mosquitto broker on a Linux machine](http://pdacontrolen.com/installation-mosquitto-broker-mqtt-in/), which will have to stay on while testing the project, although any other MQTT broker should work fine too. For the NodeMCU clients, we will be using the PubSubClient library (full documentation [here](https://pubsubclient.knolleary.net/api.html)), that can be easily installed with Arduino IDE Library Ḿanager:

- On the IDE, select "Sketch" > "Include library" > "Manage Libraries...".
- Type "PubSubClient" on the search bar.
- Install the latest version of "PubSubClient" by Nick O'Leary library.

​​​​After installing the MQTT broker and client library, you may want to test it a little. Several MQTT examples are included under "File" > "Examples" > "PubSubClient".

###  2.3. Communicating the Zumo and NodeMCU

Now that is clear how the Zumo 32U4 and the Node MCU operate, we need to electrically connect them to allow interaction with the MQTT broker from the Zumo robot. Since the NodeMCU cannot be directly plugged into the Zumo robot like the LCD display removed, we needed to make a PCB board for every robot like the one showed in the next images: 

![](placeholder.jpg)

​​​​The connections used for the PCB are detailed below. Basically, as explained when analizing the pinout of both NodeMCU and Zumo robot, we used the RX/TX UART pair for communication (RX of one device connected to TX of the other, granting data exchange), and the GND and 5v pins from the robot to power on the NodeMCU. We additionally added two LEDs on GPIOs 5 and 13 of the NodeMCU (green and red, respectively), both optional, and used only for debugging purposes (the red LED is blinked if an error occurs, and the green one if everything was successful). 

![](EspToZumoCircuit.jpg)

**Note**: If another ESP module is being used, you may want to add some voltage regulating circuit on the PCB for granting 3.3v to power it up instead of the 5v of the Zumo robot, [like this one](https://iot-playground.com/blog/2-uncategorised/17-esp8266-wifi-module-and-5v-arduino-connection).

​​​​This way, we can send and receive data from both Zumo 32U4 and ESP8285 NodeMCU using their respective UART modules. The many functions of [Arduino Serial](https://www.arduino.cc/reference/en/language/functions/communication/serial/) help us making this communication rather easy: by simply printing a string on Zumo Serial1, we can retrieve it on the NodeMCU with a reading from its Serial. On the other hand, we can send data from the NodeMCU to the Zumo robot by printing on the NodeMCU Serial and reading from the Zumo Serial1. 

Hence, a message sent from one robot to another follows the path of the image below. As seen, the message is first sent from the origin robot to its own NodeMCU (the one it is connected to), using their respective UART modules. The NodeMCU, which is a client for MQTT, publishes that message on some topic related to the second robot (destination robot on the picture). The MQTT broker handles that message, and forwards it to the NodeMCU on the destination robot, as long as it was a client previously subscribed to the topic of that message. The flow finishes with the NodeMCU sending the message via UART once more to the destination robot, which can finally process it.

![](placeholder.jpg)

## 3. Robot's finding in depth

​​​​Now that it is clear what devices are involved in our project and how they communicate, we can inspect in detail how our implementation works. The following sections will explain the main idea of our robot's finding distributed algorithm, and review the code on the NodeMCU and the Zumo robot.

### 3.1. Algorithm's overview

​​​​As in many other distributed system algorithms, we based our robot's finding in an initial leader election. That way, a robot will have a special leader role, while the others will remain as "lost" robots. The leader will then wander around trying to find their lost siblings, achieving so by the use of the IR proximity sensors. Notice that leader and lost robots will need to exchange messages during this process (for instance, to distinguish which robot the leader found, or if the leader was seeing a robot or another object with its IR sensors). The upcoming sections will explain how our algorithm does that, showing the messages exchanged between robots and the little protocol adopted for determinating if a lost robot has been found.

### 3.1.1 Robot's finding flowchart

​​​​According to what we said earlier, our robots behave like the flowchart below states. You can see that, after performing some initial leader election, a robot takes the role of leader, while the others remain on a lost state.

![](mainFlowchart.png)

​​​​The "leader main" subroutine is detailed on the next flowchart. You can see that, after moving around a little, the leader robot checks if something is in front of it by using the IR proximity sensors. If the IR pulses do not bounce back, then the path is clear and the leader may continue walking. But, if they do, and the leader senses it, then it will know something is in front of it, having found perhaps a lost robot. Now, to determinate whether or not the leader has found a lost robot (and which one), our algorithm starts a little invented protocol, which we called "You Found Me Protocol" (UFMP). We will discuss it with more detail in a later section, but basically the UFMP involves some messaging and IR sensing between robots. The relevant part is, after finishing the UFMP, the leader will actually know if it was a lost robot or any other thing what was seen before.

​​​​That way, the leader will walk until something stands in its way and, via the UFMP, will be able to know if that something was a lost robot. Now, our algorithm simply loops this until all robots are found.

![](leaderFlowchart.png)

​​​​On the other hand, the "lost main" subroutine executed by all lost robots makes them all stand still until the leader robot finds something. At that moment, the robots will perform the lost-robot-sided version of the UFMP, in order to tell if the leader can see them or not. A lost robot will then either become found after the UFMP if the leader was really staring at it, or would remain lost waiting for the next time the leader sees something.

![](lostFlowchart.png)

​​​​To sum up, the leader will be constantly wandering around for its lost fellows, and will be able to determinate or not if someone was found using the UFMP. Since this protocol and all the previous synchronization required are based over MQTT messaging system, the next sections will list what kind of messages and topics we used, before really explaining how the UFMP works.

### 3.1.2. Messaging in our algorithm

​​​​As seen in the flowchart of the previous section, the leader and lost robots need to communicate under many different scenarios. In order to easily generate and parse those various messages, we adopted a very simple format: every message is a string containing a message type, the topic related to that message (i.e. where to publish it, or where it was received from), and a payload field, which can be filled with any other useful information. These three fields are separated by a special character delimitator (for instance, '#'), making the parsing task rather easy. 

​​​​Hence, we can create any message we want from the Zumo robot with a regular string concatenation, and send it to the NodeMCU by printing on Serial1 as already discussed on the preliminary section. The NodeMCU will then receive that message, parse it to find its topic, and publish it there via the PubSubClient library. On the other hand, if the NodeMCU receives a message from the MQTT broker instead, it will only need to print it on its Serial port. The Zumo 32U4 will be able to recover it by reading Serial1, and act accordingly based on the message's type and payload.

​​​​The different message types we used in our project are the following:

- **"ESP is ready" (ERDY)**: Sent from the ESP8285 NodeMCU to the Zumo once the wireless and MQTT connection have been established. After receiving this message, the Zumo robot will start sending messages to the NodeMCU for publishing.
- **"Subscribe to topic" (SUB)**: Used by the Zumo robot to tell the NodeMCU to subscribe to some particular MQTT topic. 
- **"Unsubscribe from topic" (UNSUB)**: Works like the SUB message, but for unsubscribing from a topic.
- **"I see you" (ICU)**: This message is sent from the leader robot to all lost robots every time the first one finds something with the proximity sensors. This way, all lost robots will know the leader may be seeing one of them, and will try to see the leader on their own. 
- **"See you too" (CU2)**: Used as a reply from the lost robots to the message above. If any lost robot sees the leader, it will reply this message to it. 
- **"See you not" (CUN)**: Similar like the above, but in a negative manner (lost robot does not see the leader). 
- **"Debug message" (DEBUG)**: Message for debugging purposes only. The NodeMCU will publish all DEBUG message's payloads to a "debug topic", which can be used to trace the state of the algorithm.
- **"No message" (NONE)**: Basically, this message type is used for telling some message is no valid at all. You may think of it as a "null type" message. For instance, this type is returned when trying to parse a message type of invalid format, or when trying to read a message from the UART module with it being empty.

### 3.1.3. MQTT topics

​​​​As seen on the preliminary section, MQTT grants message forwarding using a publisher/subscriber model based on topics. The different MQTT topics we used in our project are:

- **Leader topic**: A topic to directly communicate with the leader of the group. Basically, this topic provides an abstraction layer from the leader election, allowing lost robots to talk to it without previously knowing its identity.

- **Lost robots topic**: Used for multicasting to all lost robots. A lost robot subscribes to this topic when it turns on, and unsubscribes from it when found by the leader.

- **Found robots topic**: This topic is used for communicating with all found robots. A robot subscribes to it after being found by the leader.

- **Debug topic**: As its name suggests, this topic serves debug purposes only. 

​​​​In addition to this, all robots subscribe themselves to a unique topic according to their names. This way, we can guarantee P2P communication during the steps of the UFMP, as will be described on the next section.

### 3.1.4. UFMP in detail

​​​​Explain how does UFMP work.

### 3.2. Robot's finding implementation

​​​​With the main idea of our algorithm already understood, we can now proceed to its real implementation. All code used can be found [in the Github repository for our project](https://github.com/AlejandroGarcia95/Zumo32u4-Sist-Dist-I). Although the code is already documented and references parts of this explanation, we will discuss in the next sections the most relevant aspects of it.

### 3.2.1. Our source files

​​​​The code in our repo is splitted in two separated directories, with the code suitable for the ESP8285 NodeMCU and the Zumo 32U4 on each one. Aside from the main .ino file for both devices, you will notice some other auxiliary header files, as you could expect on any C/C++ project. These different files and their purposes are detailed below.

**Header files used with ESP8285 NodeMCU**:

- *espToZumo.h*: A header file for encapsulating communication with the Zumo robot, providing a uniform messaging interface. This file allows you to create and parse messages' type, payload and topic fields in the format previously explained, and supplies simple functions to send and receive messages to and from the Zumo 32U4. This is also the file where the different message types are defined. Its sibling is the *zumoToEsp.h* file on the Zumo32U4 robot.
- *espLedsDebug.h*: A small and simple header file for debugging purposes. It only provides a function for blinking the red and green LEDs in case of failure or success.

**Header files used with Zumo 32U4**:

- *zumoMovement.h*: This useful file defines some really easy to use functions for controlling the Zumo robot's movement, as well as some experimentally measured constants for granting that simple interface. The two main functions of this library allow you either to move the robot some distance in centimeters, or to rotate it some angle in degrees (this last thing is done by using the *turnSensor.h* header file).
- *zumoProximity.h*: Mainly used during the UFMP, the functions defined on this file control the IR proximity sensor of the Zumo robot. Its two main functions allow the robot to detect an object in front of it by transmitting IR pulses, and to detect (without emitting) if such IR pulses are being received.
- *zumoToEsp.h*: A header file for encapsulating communication with the NodeMCU. It is the Zumo robot version of the *espToZumo.h* file on the NodeMCU, with exactly the same interface defined.
- *zumoLedsDebug.h*: Small and simple header file for debugging purposes. It only provides a function for blinking the red and green LEDs in case of failure or success.

### 3.2.2. ESP8285 NodeMCU code

​​​​After reviewing this project's structure, we can now focus on the .ino file for the ESP8285 NodeMCU. Yay! You will be able to see it beginning with the next few lines:

```c++
// ---------------- WIFI CONNECTION CONSTANTS ----------------

#define WIFI_NETS 3 // Change when adding more networks

#define CONNECTION_ATTEMPTS 8
#define CONNECTION_DELAY 900

#define MQTT_SERVER_IP "192.168.0.7"
#define MQTT_SERVER_PORT 1883

const char* ssid[] = {"Telecentro-40a8", "Speedy-Fibra-BF992E", "HUAWEI P9 lite", "Add your WiFi net here"};
const char* password[] = {"DDZ2WNHZ2NKN", "98A896E433FeA5BcF544", "ipv6isgood", "And its password here"};
```

The easiest part to understand are the MQTT constants defined for the connection: MQTT_SERVER_IP and MQTT_SERVER_PORT just define the broker IP address and port the ESP will be trying to connect to. Now, the next of the constants are for satisfying Wi-Fi connectivity, as seen on the next function of the file:

```c++
/* Connects to a WiFi network available from the ssid array. 
For such, performs CONNECTION ATTEMPTS attempts for connecting,
with a CONNECTION_DELAY delay of time between them. Logs to 
serial if connection was successful or not, showing IP address.*/
void setupWifi() {
  int attempts = 0;
  for(int i = 0; i < WIFI_NETS; i++){
    serialPrint("\nTrying to connect to " + String(&ssid[i][0]));
  
    WiFi.begin(&ssid[i][0], &password[i][0]);

    while(WiFi.status() != WL_CONNECTED) {
      delay(CONNECTION_DELAY);
      attempts++;
      if(attempts == CONNECTION_ATTEMPTS) {
        serialPrint("Connection attempt failed");
        attempts = 0;
        break;
      }
    }

    if(WiFi.status() == WL_CONNECTED) {
     serialPrint("\nWiFi connected");
      serialPrint("IP address: " + String(WiFi.localIP()));
      return;
    }
      
  }

  // If here, we could connect to no network
  serialPrint("\nWiFi has never conected!");
  showLedsDebug(false);
}
```

As you can see, the function iterates over the ssid array calling the *Wifi.begin* function, thus attempting a connection. If that connection is refused CONNECTION_ATTEMPTS times, the function will try to connect to the next network on the array. This is, then, a mere trick of us for beginning the Wi-Fi connection withouth needing to change the network name and password every time we changed location. If you know you are only using one Wi-Fi network for testing the project, you may want to erase the array and just keep things simple.

Going on with our analysis, we will see the *reconnect* function, in charge of reconnecting the NodeMCU with the MQTT broker if connection gets lost. We can see such connection is done with the *client.connect* function call, with a unique espId field to identify this device from others. The two other fields are the "userId" and "password" for MQTT, which you may like to change if seeking some security level.

```c++
/* Attemtps a reconnection with MQTT if this client
connection went lost. Attempts to reconnect after
2 seconds.*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    serialPrint("Attempting MQTT connection");
    // Attempt to connect
    if (client.connect(espId, "esp8266_1", "1234")) {
      serialPrint("Connected!");
      // ... and resubscribe
      client.subscribe(espId);
    } else {
      serialPrint("Failed! rc= " + String(client.state()));
      serialPrint("Trying again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}
```

The next function in the file is the *callback* function, which (as its name suggests) will be automatically called right after the NodeMCU receives a message from the MQTT broker:

```c++
/* MQTT juicy part: callback function to be called when a
new message arrives on ANY topic.*/
void callback(char* topic, byte* payload, unsigned int length) {
  // Retrieve the message on a String object
  char tmpBuffer[50] = {0};
  for(int i = 0; i < length; i++)
    tmpBuffer[i] = (char) payload[i]; 
  String msg = tmpBuffer;
  
  // Now retrieve message type
  String msgType = getMessageType(msg);

  if(msgType == MSG_NONE){
    showLedsDebug(false);
  }
  else {
    sendToZumo(msg);
    showLedsDebug(true);
  }
}
```

​​​​Since the message received on the NodeMCU was originally sent from another Zumo robot, we can see that, after performing a little String conversion and some error checking, the function only needs to forward that message to the robot this ESP8285 NodeMCU is connected to.

​​​​Moving into the *setup* function of the .ino file, we can see the callings of the previous *setup* and *reconnect* functions, aside from some configuration of the PubSubClient library involving the *callback* and MQTT_SERVER constants:

```c++
void setup(void){
  setupLedsDebug();
  setupToZumo();
  setupWifi();
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
  delay(100);
  reconnect();
  // Tell the Zumo this ESP is ready
  sendToZumo(createMessage(MSG_ERDY, "", String(espId)));
  showLedsDebug(true);
}
```

​​​​We can see above that the NodeMCU first connects to some Wi-Fi network and then to the MQTT broker (the calls to *setupLedsDebug* and *setupToZumo* are setup functions of the header files related to the NodeMCU, described on the previous section). When finished, the device sends to the Zumo robot the message "ESP is ready", and becomes available for use.

​​​​This way, we can finally look into the *loop* function code, which will have the task of handling messages received from the Zumo 32U4 (remember messages received from the MQTT broker are already handled on the *callback* function).

```c++
void loop(void){
  // MQTT stuff handling
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // Receives a message from zumo and publishes it
  String msg = receiveFromZumo();
  String msgType = getMessageType(msg);
  String msgTopic = getMessageTopic(msg);
  String msgPayload = getMessagePayload(msg);
  if(msgType == MSG_NONE)
    // Happens only on error
    showLedsDebug(false);
  else {
    serialPrint("Msg received: [" + msgType + "," + msgPayload + "," + msgTopic + "]");
    // Get topic for publishing or subscribing
    char topicBuffer[20] = {0};
    msgTopic.toCharArray(topicBuffer, msgTopic.length() + 1);
    
    // Switch on msgType
    char msgBuffer[50] = {0};
    if(msgType == MSG_SUB) {
      client.subscribe(topicBuffer);
      showLedsDebug(true);
    }
    else if(msgType == MSG_DEBUG){
      msgPayload.toCharArray(msgBuffer, msgPayload.length() + 1);
      client.publish(topicBuffer, msgBuffer);
      showLedsDebug(true);
    }
    else{
      msg.toCharArray(msgBuffer, msg.length() + 1);
      client.publish(topicBuffer, msgBuffer);
      showLedsDebug(true);
    }

  }
}
```

​​​​The very simple code then performs some action based on the message type: when receiving some SUB or UNSUB message, the NodeMCU will subscribe or unsubscribe to that topic; when receiving a DEBUG message, it will publish the message payload into the debug topic; and on any other case, the NodeMCU will just act as a dispatcher, forwarding the message recieved into the message topic. 

​​​​To sum up, all the code above allows the ESP8285 NodeMCU to connect to a Wi-Fi network and the MQTT broker, and puts it in the service of the Zumo robot. The Zumo 32U4 connected to it will then be able to subscribe, unsubscribe and publish messages via MQTT using the NodeMCU as its middleman.

### 3.2.3 Zumo32U4 code

​​​​Finally, the time has come to review the .ino file's code of the Zumo 32U4. 

## 4. Closing remarks

​​​​Some really nice conclusion

## 5. Next steps

​​​​Due to several factors (especially time constraints), we had to leave many different things out of our project. These "nice to have" features include: 

- Some *geolocation system* for all robots to constantly monitor their positions. Such an improvement would make the robot's finding more optimal, since the leader will be able to move directly to their lost peers. Although we tried this at first, we quickly left it out because most ESP geolocation projects used the Google Maps API, with distance errors considerably bigger than the robot's size.

- An algorithm's modification according to some *faul tolerant* approach (considering more robots are involved). Like in all distributed systems, the devices and the network involved are not perfect, and an unexpected error would turn the whole system useless. An easy way of start managing it may be MQTT "keep alive" and "last will" messages, which can help to automatically track any robot going down. That way, for instance, any found robot could carry on finding their lost mates if the leader went dead of batteries, or the leader would not need to wander around forever trying to find a robot whose connection got lost.

- A *leader-following* scheme for all found robots, in the sense of a little swarm algorithm. Initially, we wanted all found robots to follow the leader steps like in a conga line (you can actually find unused but defined message types for this in our code), but this resulted difficult for us in practice. Adding this would be really nice, since it could allow all robots to perform some task after being gathered by their leader.

- Some minor *security issues*, including MQTT user id and password, robots being able to logging in and out, messaging encryption, etc.

Alternatively, one could take advantage of the communicational habilities of the NodeMCU combined with MQTT and the many sensors of the Zumo robot to craft some other cool projects. Here are some ideas we came up with:

- A leader robot performs some task, and asks help "on demand" to the other robots. For example, the leader pushes away some obstacles on its own, until it finds something so big that help from its mates is required.
- The robots adopt some formation around a valuable item they need to protect from an enemy. If someone gets too close, the closest robot will push them away. If any robot "falls in combat", the others should rearrange themselves to keep the formation stable.
- All robots carry some sensor for measuring anything that would be dangerous for human beings, and coordinate to cover the whole "riskful area".  Combined with geolocation this could be used, for instance, for tracking gas leaks on buildings or other closed spaces.

