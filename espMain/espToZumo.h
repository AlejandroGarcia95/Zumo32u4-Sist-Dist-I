#pragma once

#include <Wire.h>
#include <ESP8266WiFi.h>

/* ----------------------------------------------------------------
DISCLAIMER: This library uses the ESP's RX/TX UART pins (Serial 
on the IDE) for sending and receiving messages to and from the 
Zumo robot. If you want to use them for debugging purposes, use
instead the serialPrint function.
---------------------------------------------------------------- */


/* Separators for our weirdly formatted messages.
All valid messages have the next format:
HEADER + msgType + SEP_1 + payload + SEP_2 + topic + MSG_FOOTER
where + means string concatenation.*/

const String MSG_HEADER = "@";
const String MSG_SEP_1 = "$";
const String MSG_SEP_2 = "#";
# define MSG_FOOTER '%'
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
void setupToZumo();

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

/* Sends via Serial the given message to the Zumo robot. Note 
this function DOES NOT check the message format.*/
void sendToZumo(String msg);

/* Receives a message via Serial from the Zumo robot. This function
is NOT blocking, so if no valid message was received, it will return
an empty payload MSG_NONE String. If there was a valid message, then
it returns it as a String.*/
String receiveFromZumo();

/* Use this instead of Serial.println for debugging purposes!*/
void serialPrint(String s);
