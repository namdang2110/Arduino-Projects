#include <ESP8266WiFi.h>

const char* ssid = "TANG 2"; 
const char* password = "68686868";

WiFiClient client;

int sensorPin = A0;
int enable2 = 13;
int sensorValue2 = 0;

void setup() 
{
  pinMode(enable2, OUTPUT);  
  Serial.begin(115200);
  delay(10);
  
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.println();
  Serial.print("Dang ket noi");
  Serial.println(ssid);
  Serial.print("..........");
  Serial.println();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
  Serial.println("Da ket noi Wifi");
  Serial.println();
}

void loop() 
{
  delay(500);
  sensorValue2 = analogRead(sensorPin);
  sensorValue2 = constrain(sensorValue2, 150, 440); 
  sensorValue2 = map(sensorValue2, 150, 440, 1023, 0); 
  if (sensorValue2>= 20)
  {
    Serial.print("Troi mua !");
    
    digitalWrite(enable2, HIGH);
  }
  else 
  {
    Serial.print("Khong co mua");
    digitalWrite(enable2, LOW); 
  }
  Serial.println();
  delay(100);
}