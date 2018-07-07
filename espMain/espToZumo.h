#pragma once

#include <Wire.h>
#include <ESP8266WiFi.h>

/* ----------------------------------------------------------------
DISCLAIMER: This library uses the ESP's RX/TX UART pins (Serial 
on the IDE) for sending and receiving messages to and from the 
Zumo robot. You will still be able to use the Serial for 
debugging purposes, as long as you respect this library 
initialization and don't mess with message's formats.
---------------------------------------------------------------- */


/* Separators for our weirdly formatted messages.
All valid messages have the format:
MSG_HEADER + MsgType + MSG_SEP + Payload + MSG_FOOTER
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

// Commands from Esp to Zumo
const String MSG_MOVE = "MOVE";
const String MSG_ROTATE = "ROTATE";

/*------------------------------------------------------
      Functions
------------------------------------------------------*/

// Must be called inside setup
void setupToZumo();

/* Creates and returns a message with the given type and
payload as String.*/
String createMessage(String msgType, String payload);

/* Retrieves the message type field from the given message.
If the message has an incorrect format, returns MSG_NONE.*/
String getMessageType(String msg);

/* Retrieves the message payload field from the given message.
If msg has an incorrect format, returns an empty string.*/
String getMessagePayload(String msg);

/* Sends via Serial the given message to the Zumo robot. Note 
this function DOES NOT check the message format.*/
void sendToZumo(String msg);

/* Receives a message via Serial from the Zumo robot. Since there
could be no availale message from the Zumo when this function is
called, you may like to wait for one to arrive or not. The boolean
isBlocking is used for this. Returns the received message, or an 
empty payload MSG_NONE string if isBlocking was false and there was
no message available to retrieve.*/
String receiveFromZumo(bool isBlocking);
