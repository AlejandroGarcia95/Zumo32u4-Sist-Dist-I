#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

/* Separators for our weirdly formatted messages.
All valid messages have the format:
MSG_HEADER + MsgType + MSG_SEP + Payload + MSG_FOOTER + Topic
where + means string concatenation.*/

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

// MQTT commands
const String MSG_SUB = "SUBS";

// Commands from Esp to Zumo
const String MSG_MOVE = "MOVE";
const String MSG_ROTATE = "ROTATE";

/*------------------------------------------------------
      Functions
------------------------------------------------------*/

// Must be called inside setup
void setupToEsp();

/* Creates and returns a message with the given type, payload
and topic as String.*/
String createMessage(String msgType, String payload, String topic);

/* Retrieves the message type field from the given message.
If the message has an incorrect format, returns MSG_NONE.*/
String getMessageType(String msg);

/* Retrieves the message payload field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessagePayload(String msg);

/* Retrieves the message topic field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessageTopic(String msg);

/* Sends via Serial1 the given message to the ESP. Note 
this function DOES NOT check the message format.*/
void sendToEsp(String msg);

/* Receives a message via Serial1 from the ESP. Since there
could be no availale message from the ESP when this function 
is called, you may like to wait for one to arrive or not. The 
boolean isBlocking is used for this. Returns the received message, 
or an empty payload MSG_NONE string if isBlocking was false and 
there was no message available to retrieve.*/
String receiveFromEsp(bool isBlocking);
