#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include "Switch.h"
#include "Rele.h"
#include "Button.h"

/* 
 *  D0: LightSensor
 *  D1: Switch //has INPUT_PULLUP 
 *  D3: Relé
 *  D4: DHT11
 *  D5: Relé
 *  D7: Raindrop
 *  D8: Switch //has INPUT_PULLUP 
 */

#define NAME "backyard";

#define DHTTYPE DHT11
#define D0 16

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

DHT dht(2, DHTTYPE);

float Temperature;
float Humidity;

const int sensorMin = 0;
const int sensorMax = 1024;
int sensorRainDropPin = A0;
int enableRainDrop = 13;

// SETUP ACTUATORS
Rele rele1(14, "backyard_main_light", "backyard_main_light", 1, -1); //D5
Rele rele2(0, "backyard_door_light", "backyard_door_light", 1, -1); //D3
Button switch1(12); //D6
Button switch2(5); //D1


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


void handleTemperature() {
  float Temperature = dht.readTemperature();
  String message = "{";
  message += " \"value\":";
  message += "\"" + String(Temperature) + "\"";
  message += " }";
  server.send(200, "application/json", message);
}

void handleHumidity() {
  float h = dht.readHumidity();
  String message = "{";
  message += " \"value\":";
  message += "\"" + String(h) + "\"";
  message += " }";
  server.send(200, "application/json", message);
}

void handleRainSensor() {
  String message = "{";
  message += " \"value\":";
  message += "\"" + raindropSensor() + "\"";
  message += " }";
  server.send(200, "application/json", message);
}


void handleLightSensor() {
  String message = "{";
  message += " \"value\":";
  message += "\"" + String(lightSensor()) + "\"";
  message += " }";
  server.send(200, "application/json", message);
}

void handlePing() {

  float t = dht.readTemperature();
  float h = dht.readHumidity();

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


  // Sensors
  JsonObject sensors_0 = sensors.createNestedObject();
  sensors_0["temperature"] = t;
  JsonObject sensors_1 = sensors.createNestedObject();
  sensors_1["humidity"] = h;
  JsonObject sensors_2 = sensors.createNestedObject();
  sensors_2["lightSensor"] = String(lightSensor());
  JsonObject sensors_3 = sensors.createNestedObject();
  sensors_3["raindrop"] = String(raindropSensor()) ;
  
  
  String output = "";
  serializeJson(doc, output);
  server.send(200, "application/json", output);

}

void setupRaindropSensor() {
  pinMode(enableRainDrop, OUTPUT);
}

String raindropSensor() {
  int sensorRaindDropValue = analogRead(sensorRainDropPin);
  sensorRaindDropValue = map(sensorRaindDropValue, sensorMin, sensorMax, 0, 3);
  String sensorRaindDropMsg = "";

  Serial.println(sensorRaindDropValue); 
  
  switch (sensorRaindDropValue) {
   case 0:
      Serial.println("Flood");
      sensorRaindDropMsg = "flood";
      break;
   case 1:
      Serial.println("Rain Warning");
      sensorRaindDropMsg = "rain_warning";
      break;
   case 2:
      Serial.println("Not Raining");
      sensorRaindDropMsg = "not_raining";
      break;
    case 3:
      Serial.println("very_dry_not_raining");
      sensorRaindDropMsg = "very_dry_not_raining";
      break;
    }

    Serial.println(sensorRaindDropMsg); 

    return sensorRaindDropMsg;

}

int lightSensor() {
 int light = digitalRead(D0);
 return light;
}

void handleMainLight() {
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

void handleDoorLight() {
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

void handleMainLightValue() {
  int val = rele1.getState();
  server.send(200, "application/json", "{ \"value\": " + String(val) + "}");
}

void handleDoorLightValue() {
  int val = rele2.getState();
  server.send(200, "application/json", "{ \"value\": " + String(val) + "}");
}

void setup() {

  Serial.begin(9600);
  
  pinMode(D0,INPUT);
  //setupRaindropSensor();
  dht.begin();
  
  setupWifi();
  server.on("/ping", handlePing);

  

  // Actuators
  server.on("/" + rele1.name, HTTP_POST, handleMainLight);
  server.on("/" + rele2.name, HTTP_POST, handleDoorLight);
  server.on("/" + rele1.name, HTTP_GET, handleMainLightValue);
  server.on("/" + rele2.name, HTTP_GET, handleDoorLightValue);

  // Sensors
  server.on("/temperature", handleTemperature);
  server.on("/umidity", handleHumidity);
  server.on("/rain", handleRainSensor);
  server.on("lightSensor", handleLightSensor);

  server.begin();
  Serial.println("SERVER BEGIN!!");
 
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

void loop() {
  handleSwitch();
  server.handleClient();
}
