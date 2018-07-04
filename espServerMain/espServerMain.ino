#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "espLedsDebug.h"
#include "espToZumo.h"

const char* ssid = "Speedy-Fibra-BF992E";
const char* password = "98A896E433FeA5BcF544";

ESP8266WebServer server(8080);

void handleRoot() {
  server.send(200, "text/plain", "Hello from ESP8285!\n");
  showLedsDebug(true);
}

void handleRotate() {
  String msg = createMessage(MSG_ROTATE, server.arg(0));
  sendToZumo(msg);
  server.send(200, "text/plain", "Rotating " + server.arg(0) + " degs\n");
  showLedsDebug(true);
}


void handleMove() {  
  String msg = createMessage(MSG_MOVE, server.arg(0));
  sendToZumo(msg);
  server.send(200, "text/plain", "Moving " + server.arg(0) + " cm\n");
  showLedsDebug(true);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  showLedsDebug(false);
}

void setup(void){
  setupLedsDebug();
  setupToZumo();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  //Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/rotate", handleRotate);
  server.on("/move", handleMove);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  showLedsDebug(true);
}

void loop(void){
  server.handleClient();
}
