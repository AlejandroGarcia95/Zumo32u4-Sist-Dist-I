/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define SSID_NAME "wifi net name"
#define SSID_PASS "wifi password"

#define SERVER_IP "http://192.168.1.12"

#define ENDPOINT_ROTATE "/rotate"
#define ENDPOINT_MOVE "/move"

ESP8266WiFiMulti WiFiMulti;

void setup() {

    Serial.begin(9600);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("SSID", "PASSWORD");

}

void makeRequestToURI(String reqUri){
  // wait for WiFi connection
  if((WiFiMulti.run() == WL_CONNECTED)) {

      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
      // configure traged server and url
      //http.begin(reqUri, "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
      http.begin(reqUri); //HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if(httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          Serial.printf("[HTTP] GET... code: %d\n", httpCode);

          // file found at server
          if(httpCode == HTTP_CODE_OK) {
              String payload = http.getString();
              Serial.println(payload);
          }
      } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
  }
}

void loop() {

    String zumoBtn = Serial.readString();
    if(zumoBtn == "A")
      // Move 5cm forward
      makeRequestToURI(String(SERVER_IP) + String(ENDPOINT_MOVE) + "?d=5");
    if(zumoBtn == "B")
      // Move 5cm backwards
      makeRequestToURI(String(SERVER_IP) + String(ENDPOINT_MOVE) + "?d=-5");
    if(zumoBtn == "C")
      // Rotate 45 degrees
      makeRequestToURI(String(SERVER_IP) + String(ENDPOINT_ROTATE) + "?a=45");
}

