#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include <ArduinoJson.h>
#include "DHT.h"

#define WIFI_SSID "TANG 2"
#define WIFI_PASSWORD "68686868"
#define DHTPIN 5
#define DHTTYPE DHT22

#define FIREBASE_HOST "iot-plant-174a3-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyC-0htnksKwJ2nf4Y2HaFpNJZ2tcq0_xFk"

DHT dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;
//WiFiClient client;

String path = "/";
FirebaseJson json;

int sensorPin = A0;
int enable2 = 13;
int sensorValue2 = 0;

void setup() 
{
  Serial.begin(9600);
  dht.begin();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");               
  }   
    
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
    
  if(!Firebase.beginStream(firebaseData, path))
  {
      Serial.println("REASON : " + firebaseData.errorReason());
      Serial.println();
  }     
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();  
}
 
void loop() 
{
  delay(500);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) 
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  float hif = dht.computeHeatIndex(f, h);

  float hic = dht.computeHeatIndex(t, h, false);

  Firebase.setString(firebaseData, path + "/data", h);  
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  delay(100);
}