#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "espLedsDebug.h"
#include "espToZumo.h"

// ---------------- WIFI CONNECTION CONSTANTS ----------------

#define WIFI_NETS 2 // Change when adding more networks

#define CONNECTION_ATTEMPTS 8
#define CONNECTION_DELAY 900

#define MQTT_SERVER_IP "192.168.1.50"
#define MQTT_SERVER_PORT 1883

const char* ssid[] = {"HUAWEI P9 lite", "Speedy-Fibra-BF992E", "Add your WiFi net here"};
const char* password[] = {"ipv6isgood", "98A896E433FeA5BcF544", "And its password here"};

const char* myId = "ESP8266_1";
const char* otherId = "ESP8266_2";

WiFiClient espClient;
PubSubClient client(espClient);

/* Connects to a WiFi network available from the ssid array. 
For such, performs CONNECTION ATTEMPTS attempts for connecting,
with a CONNECTION_DELAY delay of time between them. Logs to 
serial if connection was successful or not, showing IP address.*/
void setupWifi() {
  int attempts = 0;
  for(int i = 0; i < WIFI_NETS; i++){
    Serial.print("\nTrying to connect to ");
    Serial.println(&ssid[i][0]);
  
    WiFi.begin(&ssid[i][0], &password[i][0]);

    while(WiFi.status() != WL_CONNECTED) {
      delay(CONNECTION_DELAY);
      attempts++;
      if(attempts == CONNECTION_ATTEMPTS) {
        Serial.println("Connection attempt failed");
        attempts = 0;
        break;
      }
    }

    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return;
    }
      
  }

  // If here, we could connect to no network
  Serial.println("\nWiFi has never conected!");
  showLedsDebug(false);
}

/* Attemtps a reconnection with MQTT if this client
connection went lost. Attempts to reconnect after
5 seconds. TODO: Adapt this on future*/
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(myId)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(myId);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
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
  String msg  = tmpBuffer;
  
  // Now retrieve message type and act accordingly
  String msgType = getMessageType(msg);
  
  if((msgType == MSG_MOVE) || (msgType == MSG_ROTATE)) {
    sendToZumo(msg);
    showLedsDebug(true);
  }
  else if(msgType == MSG_DEBUG)
    Serial.println(msg);
  else {
    Serial.println(msg);
    showLedsDebug(false);
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
  showLedsDebug(true);
}

void loop(void){
  // MQTT stuff handling
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Receives a message from zumo and publishes it
  String msg = receiveFromZumo(false);
  String msgType = getMessageType(msg);
  if(msgType == MSG_NONE)
    return;
  else if((msgType == MSG_MOVE) || (msgType == MSG_ROTATE)) {
    char tmpBuffer[50] = {0};
    msg.toCharArray(tmpBuffer, msg.length() + 1);
    client.publish(otherId, tmpBuffer);
    showLedsDebug(true);
  }
  else if(msgType == MSG_DEBUG)
    Serial.println(msg);
  else {
    Serial.println(msg);
    showLedsDebug(false);
  }
  
}
