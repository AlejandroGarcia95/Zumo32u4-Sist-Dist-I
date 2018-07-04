#include <Wire.h>
#include <ESP8266WiFi.h>

#include "espToZumo.h"

// Must be called inside setup
void setupToZumo() {
  Serial.begin(9600);
  //pinMode(0, INPUT_PULLUP);
  Serial.flush();
  Serial.readString();
}

String createMessage(String msgType, String payload){
  String msg = msgType + String(MSG_SEP) + payload;
  return (String(MSG_HEADER) +  msg + String(MSG_FOOTER));
}

String getMessageType(String msg){
  int indexHeader = msg.indexOf(String(MSG_HEADER));
  int indexSep = msg.indexOf(String(MSG_SEP));
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep < 0) || (indexFooter < 0))
    return MSG_NONE;

  return msg.substring(indexHeader + 1, indexSep);
}

String getMessagePayload(String msg){
  int indexHeader = msg.indexOf(String(MSG_HEADER));
  int indexSep = msg.indexOf(String(MSG_SEP));
  int indexFooter = msg.indexOf(String(MSG_FOOTER));
  if((indexHeader < 0) || (indexSep < 0) || (indexFooter < 0))
    return MSG_NONE;

  return msg.substring(indexSep + 1, indexFooter);
}

void sendToZumo(String msg){
  Serial.println(msg);
}

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


