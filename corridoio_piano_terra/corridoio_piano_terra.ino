#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include "Switch.h"
#include "Rele.h"

#define NAME "corridoio_piano_terra";

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
// END SETUP WIFI

// SETUP ACTUATORS AND SENSORS
Switch switch1(14); //D2
Switch switch2(2); //D4
Rele rele1(5, "luce_giardino", "luce_giardino", 1, 12); //D1 rele //D5 Led
Rele rele2(0, "luce_porta", "luce_porta", 1, 13); //D3 Rele //D6 Led
Rele rele3(15, "anti_zanzare", "anti_zanzare", 1, -1); //D3 Rele //D6 Led

String IpAddress2String(const IPAddress& ipAddress){
  return String(ipAddress[0]) + String(".") +\
    String(ipAddress[1]) + String(".") +\
    String(ipAddress[2]) + String(".") +\
    String(ipAddress[3]);
}


void handlePing() {

  // HOW TO CONVERT JSON QUICKLY: https://arduinojson.org/v6/assistant/
  
  DynamicJsonDocument doc(2048);
  doc["deviceName"] = deviceName;
  doc["ip"] = ip;
  JsonArray sensors = doc.createNestedArray("sensors");
  JsonArray actuators = doc.createNestedArray("actuators");
  
  //rele 1
  JsonObject actuators_0 = actuators.createNestedObject();
  actuators_0["name"] = rele1.name;
  actuators_0["alias"] = rele1.name;

  JsonArray actuators_0_range = actuators_0.createNestedArray("range");
  actuators_0_range.add("0");
  actuators_0_range.add("1");
  actuators_0["step"] = 1;
  actuators_0["value"] = rele1.getState();

  //rele 2
  JsonObject actuators_1 = actuators.createNestedObject();
  actuators_1["name"] = rele2.name;
  actuators_1["alias"] = rele2.name;

  JsonArray actuators_1_range = actuators_1.createNestedArray("range");
  actuators_1_range.add("0");
  actuators_1_range.add("1");
  actuators_1["step"] = 1;
  actuators_1["value"] = rele2.getState();
  
  
  String output = "";
  serializeJson(doc, output);
  server.send(200, "application/json", output);
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

void handleLuceGiardino() {
  String value = server.arg("plain");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, value);

  String val = doc["value"];

  if (val == "1") {
      rele1.on();
  }else{
    rele1.off();
  }
  
  server.send(200, "application/json", "{ \"value\": \""+  val + "\"}");
}

void handleLucePorta() {
  String value = server.arg("plain");
  StaticJsonDocument<256> doc;
  deserializeJson(doc, value);

  String val = doc["value"];

  if (val == "1") {
    rele2.on();
  }else{
    rele2.off();
  }
  
  server.send(200, "application/json", "{ \"value\": \""+  val + "\"}");
}

void handleLuceGiardinoValue() {
  int val = rele1.getState();
  server.send(200, "application/json", "{ \"value\": " + String(val) + "}");
}

void handleLucePortaValue() {
  int val = rele2.getState();
  server.send(200, "application/json", "{ \"value\": " + String(val) + "}");
}


void toggleRele1() {
  rele1.toggle();
}

void toggleRele2() {
  rele2.toggle();
}

void handleSwitch() {
  switch1.onChange(toggleRele1);
  switch2.onChange(toggleRele2);
}

void setup() {

  Serial.begin(9600);
  setupWifi();

  server.on("/ping", handlePing);
  server.on("/luce_giardino", HTTP_POST, handleLuceGiardino);
  server.on("/luce_porta", HTTP_POST, handleLucePorta);
  server.on("/luce_giardino", HTTP_GET, handleLuceGiardinoValue);
  server.on("/luce_porta", HTTP_GET, handleLucePortaValue);

  server.begin();
  Serial.println("SERVER BEGIN!!");
}

void loop() {
  handleSwitch();
  server.handleClient();
}
