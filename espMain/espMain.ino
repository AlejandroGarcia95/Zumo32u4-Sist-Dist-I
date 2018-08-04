#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "espLedsDebug.h"
#include "espToZumo.h"

// ---------------- WIFI CONNECTION CONSTANTS ----------------

#define WIFI_NETS 4 // Change when adding more networks

#define CONNECTION_ATTEMPTS 8
#define CONNECTION_DELAY 900

//#define MQTT_SERVER_IP "192.168.1.15"
//#define MQTT_SERVER_IP "192.168.0.7"
#define MQTT_SERVER_IP "192.168.1.50"
#define MQTT_SERVER_PORT 1883

const char* ssid[] = {"Speedy-Fibra-BF992E", "HUAWEI P9 lite", "WiFi-Arnet-F-2.4", "Telecentro-40a8", "Add your WiFi net here"};
const char* password[] = {"98A896E433FeA5BcF544", "ipv6isgood", "juanca01", "DDZ2WNHZ2NKN", "And its password here"};

const char espId[] = "Cassandra";

WiFiClient espClient;
PubSubClient client(espClient);

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

/* Attemtps a reconnection with MQTT if this client
connection went lost. Attempts to reconnect after
5 seconds. TODO: Adapt this on future*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    serialPrint("Attempting MQTT connection");
    // Attempt to connect
    if (client.connect(espId, "esp8266_1", "1234")) {
      serialPrint("Connected!");
      // ... and resubscribe
      client.subscribe(espId);
      //client.publish("debug", "ESP IS UP");
    } else {
      serialPrint("Failed! rc= " + String(client.state()));
      serialPrint("Trying again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

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


// ----------------------- MAIN PROGRAM -----------------------

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
    else if(msgType == MSG_UNSUB) {
      client.unsubscribe(topicBuffer);
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

  // Save some battery
  delay(100);
}
