#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoToEsp.h"

// Must be called inside setup
void setupToEsp() {
  Serial1.begin(115200);
  pinMode(0, INPUT_PULLUP);
  Serial1.flush();
  Serial1.readString();
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
    return MSG_NONE;

  return msg.substring(indexSep + 1, indexFooter);
}

/* Sends via Serial1 the given message to the ESP. Note 
this function DOES NOT check the message format.*/
void sendToEsp(String msg){
  Serial1.println(msg);
}

/* Receives a message via Serial1 from the ESP. Since there
could be no availale message from the ESP when this function 
is called, you may like to wait for one to arrive or not. The 
boolean isBlocking is used for this. Returns the received message, 
or an empty payload MSG_NONE string if isBlocking was false and 
there was no message available to retrieve.*/
String receiveFromEsp(bool isBlocking){
  String msg = createMessage(MSG_NONE, "");

  do {

    if(Serial1.available() > 2) {
      msg = Serial1.readString();
      isBlocking = false;
    }

  } while(isBlocking);

  return msg;
}


