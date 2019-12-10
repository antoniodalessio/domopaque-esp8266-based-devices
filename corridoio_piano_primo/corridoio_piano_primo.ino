/*
  DHT11
  BLU: GND
  VIOLA: 3,3
  VERDE: D4
*/

#include <ESP8266WebServer.h>

#include "DHT.h"
#define DHTTYPE DHT11
#define DHT11_PIN 2

#ifndef STASSID
#define STASSID "Vodafone-50278807"
#define STAPSK  "adhbm3yv8j2lx3i"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const uint16_t port = 3005;

DHT dht(DHT11_PIN, DHTTYPE);

float Temperature;
float Humidity;

String ip;

// MAgnetic sensor
int magneticSensorPin = 4;
int buttonState = 0;


const int sensorMin = 0;
const int sensorMax = 1024;
int sensorRainDropPin = A0;
int enableRainDrop = 13;



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

String jsonKeyValue(String key, String value, boolean final) {
  String str = "";
  str += " \"" + key + "\":";
  if (final) {
    str += " \"" + value + "\"";
  }else{
    str += " \"" + value + "\",";
  }
  return str;
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

void handlePing() {

  float Temperature = dht.readTemperature();
  float h = dht.readHumidity();

  String message = "{ ";

    message += jsonKeyValue("deviceName", "corridoio_piano_primo", false);
    message += jsonKeyValue("ip", ip, false);

    message += " \"sensors\":[";

    message += "{";
    message += jsonKeyValue("temperature", String(Temperature) , true);
    message += "},";

    message += "{";
    message += jsonKeyValue("umidity", String(h) , true);
    message += "}";
    
    message += "],";

    message += " \"actuators\":[";

    message += "]";

    message += " }";
  
  Serial.println("SERVER RESPONSE");
  server.send(200, "application/json", message);
}

void magneticSensor() {
  buttonState = digitalRead(magneticSensorPin);
  if (buttonState == HIGH) {
     Serial.println("high");
     Serial.println(buttonState);
  } else {
    Serial.println("low");
     Serial.println(buttonState);
  }
 
  delay(1000);
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

void setup() {
  //setupRaindropSensor();
  
  //pinMode(magneticSensorPin,INPUT_PULLUP);  
  
  Serial.begin(9600);
  setupWifi();
  dht.begin();

  server.on("/ping", handlePing);
  server.on("/temperature", handleTemperature);
  server.on("/umidity", handleHumidity);
  
  server.begin();
  Serial.println("SERVER BEGIN!!");
}

void loop() {
  server.handleClient();
  //magneticSensor();
  //raindropSensor();
}
