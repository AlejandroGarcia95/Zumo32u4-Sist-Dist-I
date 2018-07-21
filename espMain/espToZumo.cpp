#include <Wire.h>
#include <ESP8266WiFi.h>

#include "espToZumo.h"

// Must be called inside setup
void setupToZumo(){
  Serial.begin(115200); // Must be the same value as in Zumo
  //pinMode(0, INPUT_PULLUP);
  Serial.flush();
  Serial.readString();
}

/* Creates and returns a message with the given type, payload
and topic as String.*/
String createMessage(String msgType, String payload, String topic){
  String msg = msgType + MSG_SEP_1 + payload + MSG_SEP_2 + topic;
  return (MSG_HEADER +  msg + String(MSG_FOOTER));
}

/* Retrieves the message type field from the given message.
If the message has an incorrect format, returns MSG_NONE.*/
String getMessageType(String msg){
  int indexHeader = msg.indexOf(MSG_HEADER);
  int indexSep1 = msg.indexOf(MSG_SEP_1);
  int indexSep2 = msg.indexOf(MSG_SEP_2);
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep1 < 0) || (indexSep2 < 0) || (indexFooter < 0))
    return MSG_NONE;

  return msg.substring(indexHeader + 1, indexSep1);
}

/* Retrieves the message payload field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessagePayload(String msg){
  int indexHeader = msg.indexOf(MSG_HEADER);
  int indexSep1 = msg.indexOf(MSG_SEP_1);
  int indexSep2 = msg.indexOf(MSG_SEP_2);
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep1 < 0) || (indexSep2 < 0) || (indexFooter < 0))
    return "";

  return msg.substring(indexSep1 + 1, indexSep2);
}

/* Retrieves the message topic field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessageTopic(String msg){
  int indexHeader = msg.indexOf(MSG_HEADER);
  int indexSep1 = msg.indexOf(MSG_SEP_1);
  int indexSep2 = msg.indexOf(MSG_SEP_2);
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep1 < 0) || (indexSep2 < 0) || (indexFooter < 0))
    return MSG_NONE;

  return msg.substring(indexSep2 + 1, indexFooter);
}

/* Sends via Serial the given message to the Zumo robot. Note 
this function DOES NOT check the message format.*/
void sendToZumo(String msg){
  Serial.println(msg);
  delay(5);
}

/* Receives a message via Serial from the Zumo robot. This function
is NOT blocking, so if no valid message was received, it will return
an empty payload MSG_NONE String. If there was a valid message, then
it returns it as a String.*/
String receiveFromZumo(){
  String msg = createMessage(MSG_NONE, "", "");

  if(Serial.available() > 4)
    msg = Serial.readStringUntil(MSG_FOOTER);

  msg = msg + String(MSG_FOOTER);
  return msg;
}

/* Use this instead of Serial.println for debugging purposes!*/
void serialPrint(String s){
   Serial.println(s + String(MSG_FOOTER));
}

