#include <Servo.h>
#include "Switch.h"
#include "Rele.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>


//BEGIN SETUP WIFI
#ifndef STASSID
#define STASSID "Vodafone-50278807"
#define STAPSK  "adhbm3yv8j2lx3i"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;
const uint16_t port = 3011;
String ip;

Servo servo;
Switch button1(14); //D2
Rele rele1(5, "acqua", "acqua", 1, -1); //D1 rele

ESP8266WebServer server(port);



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



void setup() {
  Serial.begin(115200);
  setupWifi();
  rele1.off();
  servo.attach(2); //D4
  servo.write(0); //sportello chiuso

  server.on("/feed", feed);
  server.on("/quench", quench);

  server.begin();
}


void quench() {
  rele1.on();
  delay(2000);
  rele1.off();
  server.send(200, "application/json", "{}");
}

void feed() {
  //servo.write(90);//sportello aperto
  //delay(2000);
  //servo.write(0);
  server.send(200, "application/json", "{}");
}

void loop() {
  server.handleClient();
  //button1.onChange(feed);
}
