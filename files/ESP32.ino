// DHT Temperature & Humidity Sensor
// Unified Sensor Library Example
// Written by Tony DiCola for Adafruit Industries
// Released under an MIT license.

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#define LED 2

const char *AP_SSID = "Lumia";//"2.4G-PrincessK";
const char *AP_PWD = "alamakota";
String serverName = "https://ramida3.pythonanywhere.com/device/1"; 


WiFiMulti wifiMulti;

#define LED_GPIO 2
#define DHTPIN 4     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

void setup() {
  Serial.begin(9600);
    pinMode(LED_GPIO, OUTPUT);
  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

    delay(4000);
  wifiMulti.addAP(AP_SSID, AP_PWD);

  
}

void loop() {
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
    postDataToServer(event.temperature);
    delay(5000);
    getDataFromServer();
    
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
}


void getDataFromServer()
{
   
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      String serverPath = serverName ;
      http.begin(serverPath.c_str());
      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        
        String payload = http.getString();
        Serial.println(payload);
        Serial.println(payload.indexOf('switch'));
        
        Serial.println(payload.substring(payload.indexOf('switch')+3,payload.indexOf('switch')+4));

        if (payload.substring(payload.indexOf('switch')+3,payload.indexOf('switch')+4)=="1")
        digitalWrite(LED_GPIO, 1);
        else
        digitalWrite(LED_GPIO, 0);
        
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
   
}
 

void postDataToServer() {
 
  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  if (wifiMulti.run() == WL_CONNECTED) {
     
    HTTPClient http;   
     
    http.begin("https://ramida3.pythonanywhere.com/mes");  
    http.addHeader("Content-Type", "application/json");         
     
    StaticJsonDocument<200> doc;
  
    doc["time_stamp"] = "2022-05-20T11:10:00";
    doc["value"] = 2.2;
    doc["device_id"] =   1;
   
      
    String requestBody;
    serializeJson(doc, requestBody);

    Serial.println(requestBody);
     Serial.println("Wyslany");
     
    int httpResponseCode = http.POST(requestBody);

    delay(100);
    
    if(httpResponseCode>0){
       
      String response = http.getString();                       
       
      Serial.println(httpResponseCode);   
      Serial.println(response);
     
    }
    else {
     
      //Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(statusCode).c_str());
       
    }
     
  }
}

void postDataToServer(float dane) {
 
  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  if (wifiMulti.run() == WL_CONNECTED) {
     
    HTTPClient http;   
     
    http.begin("https://ramida3.pythonanywhere.com/mes");  
    http.addHeader("Content-Type", "application/json");         
     
    StaticJsonDocument<200> doc;
  
    doc["time_stamp"] = "2022-05-20T11:10:00";
    doc["value"] = dane;
    doc["device_id"] =   1;
   
      
    String requestBody;
    serializeJson(doc, requestBody);

    Serial.println(requestBody);
     Serial.println("Wyslany");
     
    int httpResponseCode = http.POST(requestBody);

    delay(100);
    
    if(httpResponseCode>0){
       
      String response = http.getString();                       
       
      Serial.println(httpResponseCode);   
      Serial.println(response);
     
    }
    else {
     
      //Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(statusCode).c_str());
       
    }
     
  }
}
