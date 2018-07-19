#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoToEsp.h"

// Must be called inside setup
void setupToEsp(){
  Serial1.begin(115200); // Must be the same value as in ESP
  pinMode(0, INPUT_PULLUP);
  Serial1.flush();
  Serial1.readString();
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

/* Sends via Serial1 the given message to the ESP. Note 
this function DOES NOT check the message format.*/
void sendToEsp(String msg){
  Serial1.println(msg);
  delay(5);  
}

/* Receives a message via Serial1 from the ESP. This function is NOT
blocking, so if no valid message was received, it will return an
empty payload MSG_NONE String. If there was a valid message, then
it returns it as a String.*/
String receiveFromEsp(){
  String msg = createMessage(MSG_NONE, "", "");

  if(Serial1.available() > 4)
    msg = Serial1.readStringUntil(MSG_FOOTER);

  msg = msg + String(MSG_FOOTER);
  return msg;
}


/* Sends a string to debug topic. */
void sendDebugMessage(String payload) {
  String msg = createMessage(MSG_DEBUG, payload, DEBUG_TOPIC);
  sendToEsp(msg);
}

