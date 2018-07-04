#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

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
void setupToEsp();

String createMessage(String msgType, String payload);

String getMessageType(String msg);

String getMessagePayload(String msg);

void sendToEsp(String msg);

String receiveFromEsp(bool isBlocking);
