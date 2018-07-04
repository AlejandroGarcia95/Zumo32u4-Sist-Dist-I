#include <Wire.h>
#include <Zumo32U4.h>

#include "zumoToEsp.h"

// Must be called inside setup
void setupToEsp() {
  Serial1.begin(9600);
  pinMode(0, INPUT_PULLUP);
  Serial1.flush();
  Serial1.readString();
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

void sendToEsp(String msg){
  Serial1.println(msg);
}

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


