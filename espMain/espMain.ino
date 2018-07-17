#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "espLedsDebug.h"
#include "espToZumo.h"

// ---------------- WIFI CONNECTION CONSTANTS ----------------

#define WIFI_NETS 3 // Change when adding more networks

#define CONNECTION_ATTEMPTS 8
#define CONNECTION_DELAY 900

#define MQTT_SERVER_IP "192.168.0.7"
#define MQTT_SERVER_PORT 1883

const char* ssid[] = {"Telecentro-40a8", "HUAWEI P9 lite", "Speedy-Fibra-BF992E","Add your WiFi net here"};
const char* password[] = {"DDZ2WNHZ2NKN", "ipv6isgood", "98A896E433FeA5BcF544", "And its password here"};

const char espId[] = "Neo";

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
    if (client.connect(espId, "esp8266_1", "1234")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(espId);
      client.publish("debug", "Hello from ESP!");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 5 seconds before retrying
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
  String msg  = tmpBuffer;
  
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
  String msgTopic = getMessageTopic(msg);
  if(msgType == MSG_NONE) {
    // Should not happen
    showLedsDebug(false);
  }
  else {
    Serial.println("Msg received is: " + msgType + " " + getMessagePayload(msg) + " " + msgTopic + ".");
    if(msgType == MSG_SUB) {
      char topicBuffer[20] = {0};
      msgTopic.toCharArray(topicBuffer, msgTopic.length() + 1);
      client.subscribe(topicBuffer);
      showLedsDebug(true);
    }  
    else {
      char msgBuffer[50] = {0};
      char topicBuffer[20] = {0};
      msg.toCharArray(msgBuffer, msg.length() + 1);
      msgTopic.toCharArray(topicBuffer, msgTopic.length() + 1);
      client.publish(topicBuffer, msgBuffer);
      showLedsDebug(true);
    }
  }
}
