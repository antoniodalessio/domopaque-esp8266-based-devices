#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Rele.h"
#include "Button.h"


/*
 *  D1: Switch //has INPUT_PULLUP 
 *  D3: Relé
 *  D5: Relé
 */

#define NAME "bathroom";

//BEGIN SETUP WIFI
#ifndef STASSID
#define STASSID "Vodafone-50278807"
#define STAPSK  "adhbm3yv8j2lx3i"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
const uint16_t port = 3005;
String ip;
const char* deviceName = NAME;

ESP8266WebServer server(port);


// SETUP ACTUATORS
Rele rele1(14, "bathroom_dehumidifier", "bathroom_dehumidifier", 1, -1); //D5
Rele rele2(0, "bathroom_heater", "bathroom_heater", 1, -1); //D3
Rele rele3(0, "bathroom_air_freshener", "bathroom_air_freshener", 1, -1); //D3


String IpAddress2String(const IPAddress& ipAddress)
{
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}

void setupWifi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  ip = IpAddress2String(WiFi.localIP());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(ip);
}



void handlePing() {

  DynamicJsonDocument doc(2048);
  doc["deviceName"] = deviceName;
  doc["ip"] = ip;
  JsonArray sensors = doc.createNestedArray("sensors");
  JsonArray actuators = doc.createNestedArray("actuators");
  
  // rele 1
  JsonObject actuators_0 = actuators.createNestedObject();
  actuators_0["name"] = rele1.name;
  actuators_0["alias"] = rele1.name;

  JsonArray actuators_0_range = actuators_0.createNestedArray("range");
  actuators_0_range.add("0");
  actuators_0_range.add("1");
  actuators_0["step"] = 1;
  actuators_0["value"] = rele1.getState();

  // rele 2
  JsonObject actuators_1 = actuators.createNestedObject();
  actuators_1["name"] = rele2.name;
  actuators_1["alias"] = rele2.name;

  JsonArray actuators_1_range = actuators_1.createNestedArray("range");
  actuators_1_range.add("0");
  actuators_1_range.add("1");
  actuators_1["step"] = 1;
  actuators_1["value"] = rele2.getState();

  // rele 3
  JsonObject actuators_2 = actuators.createNestedObject();
  actuators_2["name"] = rele3.name;
  actuators_2["alias"] = rele3.name;

  JsonArray actuators_2_range = actuators_2.createNestedArray("range");
  actuators_2_range.add("0");
  actuators_2_range.add("1");
  actuators_2["step"] = 1;
  actuators_2["value"] = rele3.getState();

    
  String output = "";
  serializeJson(doc, output);
  server.send(200, "application/json", output);

}

void handleRele(Rele rele) {
  String value = server.arg("plain");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, value);

  String val = doc["value"];

  if (val == "1") {
    rele.on();
  }else{
    rele.off();
  }
  
  server.send(200, "application/json", "{ \"value\": \""+  val + "\"}");

}

void handleRele1() {
  handleRele(rele1);
}

void handleRele2() {
  handleRele(rele2);
}

void handleRele3() {
  handleRele(rele3);
}

void handleReleValue(Rele rele) {
  int val = rele.getState();
  server.send(200, "application/json", "{ \"value\": " + String(val) + "}");
}

void handleReleValue1() {
  handleReleValue(rele1);
}

void handleReleValue2() {
  handleReleValue(rele2);
}

void handleReleValue3() {
  handleReleValue(rele3);
}


void setup() {

  Serial.begin(9600);  
  
  setupWifi();
  server.on("/ping", handlePing);
  
  // Actuators
  server.on("/" + rele1.name, HTTP_POST, handleRele1);
  server.on("/" + rele2.name, HTTP_POST, handleRele2);
  server.on("/" + rele3.name, HTTP_POST, handleRele3);

  server.on("/" + rele1.name, HTTP_GET, handleReleValue1);
  server.on("/" + rele2.name, HTTP_GET, handleReleValue2);
  server.on("/" + rele3.name, HTTP_GET, handleReleValue3);
  
  server.begin();
  Serial.println("SERVER BEGIN!!");
 
}

void loop() {
  server.handleClient();
}
