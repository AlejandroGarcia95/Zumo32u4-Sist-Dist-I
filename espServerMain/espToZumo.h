#pragma once

#include <Wire.h>
#include <ESP8266WiFi.h>

#define MSG_HEADER "@"
#define MSG_SEP "$"
#define MSG_FOOTER "#"

/*------------------------------------------------------
 		Message types
------------------------------------------------------*/

// General purpose messages
const String MSG_NONE = "NONE";
const String MSG_ERROR = "ERROR";
const String MSG_DEBUG = "DEBUG";

// Commands from Esp to Zumo
const String MSG_MOVE = "MOVE";
const String MSG_ROTATE = "ROTATE";




// Must be called inside setup
void setupToZumo();

String createMessage(String msgType, String payload);

String getMessageType(String msg);

String getMessagePayload(String msg);

void sendToZumo(String msg);

String receiveFromZumo(bool isBlocking);
