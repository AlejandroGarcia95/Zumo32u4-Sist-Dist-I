#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

/* Separators for our weirdly formatted messages.
All valid messages have the next format:
HEADER + msgType + SEP_1 + payload + SEP_2 + topic + MSG_FOOTER
where + means string concatenation.*/

const String MSG_HEADER = "@";
const String MSG_SEP_1 = "$";
const String MSG_SEP_2 = "#";
# define MSG_FOOTER '%'

const String DEBUG_TOPIC = "debug";

/*------------------------------------------------------
 		Message types
------------------------------------------------------*/

// General purpose messages
const String MSG_NONE = "NONE";
const String MSG_DEBUG = "DEBUG";

// MQTT commands
const String MSG_SUB = "SUB";
const String MSG_UNSUB = "UNSUB";

// Message for telling Zumo the ESP is ready
const String MSG_ERDY = "ERDY";

// Message for leader election
const String MSG_LDRE = "LDRE";

// Messages for UFMP
const String MSG_ICU = "ICU"; // I see you
const String MSG_CU2 = "CU2"; // See you too
const String MSG_CUN = "CUN"; // See you not
const String MSG_SA = "SA"; // Start alingment
const String MSG_FA = "FA"; // Finish alingment

// Messages for FLP
const String MSG_MOVE = "MOV"; 
const String MSG_ROTATE = "ROT";

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

/* Receives a message via Serial1 from the ESP. This function is NOT
blocking, so if no valid message was received, it will return an
empty payload MSG_NONE String. If there was a valid message, then
it returns it as a String.*/
String receiveFromEsp();

/* Sends a string to DEBUG_TOPIC. */
void sendDebugMessage(String payload);
