#include <Wire.h>
#include <ESP8266WiFi.h>

#include "espToZumo.h"

// Must be called inside setup
void setupToZumo() {
  Serial.begin(115200); // Must be the same value as in Zumo
  //pinMode(0, INPUT_PULLUP);
  Serial.flush();
  Serial.readString();
}

/* Creates and returns a message with the given type and
payload as String.*/
String createMessage(String msgType, String payload){
  String msg = msgType + String(MSG_SEP) + payload;
  return (String(MSG_HEADER) +  msg + String(MSG_FOOTER));
}

/* Retrieves the message type field from the given message.
If the message has an incorrect format, returns MSG_NONE.*/
String getMessageType(String msg){
  int indexHeader = msg.indexOf(String(MSG_HEADER));
  int indexSep = msg.indexOf(String(MSG_SEP));
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep < 0) || (indexFooter < 0))
    return MSG_NONE;

  return msg.substring(indexHeader + 1, indexSep);
}

/* Retrieves the message payload field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessagePayload(String msg){
  int indexHeader = msg.indexOf(String(MSG_HEADER));
  int indexSep = msg.indexOf(String(MSG_SEP));
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep < 0) || (indexFooter < 0))
    return "";

  return msg.substring(indexSep + 1, indexFooter);
}

/* Sends via Serial the given message to the Zumo robot. Note 
this function DOES NOT check the message format.*/
void sendToZumo(String msg){
  Serial.println(msg);
}

/* Receives a message via Serial from the Zumo robot. Since there
could be no availale message from the Zumo when this function is
called, you may like to wait for one to arrive or not. The boolean
isBlocking is used for this. Returns the received message, or an 
empty payload MSG_NONE string if isBlocking was false and there was
no message available to retrieve.*/
String receiveFromZumo(bool isBlocking){
  String msg = createMessage(MSG_NONE, "");

  do {

    if(Serial.available() > 2) {
      msg = Serial.readString();
      isBlocking = false;
    }

  } while(isBlocking);

  return msg;
}


