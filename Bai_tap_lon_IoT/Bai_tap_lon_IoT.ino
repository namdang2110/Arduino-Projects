#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "DHT.h"

#define DHTPIN 4                
#define DHTTYPE DHT22

#define WIFI_SSID "Nhập vào tên Wifi"
#define WIFI_PASSWORD "Nhập vào mật khẩu Wifi"

#define FIREBASE_HOST "Nhập vào đường link dẫn đến Realtime Databse Firebase"
#define FIREBASE_AUTH "Nhập vào mã Firebase"

FirebaseData firebaseData;
//WiFiClient client;

String path = "/";
FirebaseJson json;

int sensor_pin = A0; 
const int pump = 5; 
const float threshold = 40;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Dang doc cam bien...");
  pinMode(pump, OUTPUT);
  dht.begin();

WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");               
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

  delay(2000);
  }

void loop() {

    float h = dht.readHumidity();          
    float t = dht.readTemperature();       
    float f = dht.readTemperature(true);  

    if (isnan(h) || isnan(t) || isnan(f)) 
    {
      Serial.println(F("Loi khi doc module DHT22 !"));
      return;
    }

    float hif = dht.computeHeatIndex(f, h);       
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Do am khong khi : "));
    Serial.print(h);
    Serial.print(F("%  Nhiet do : "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Chi so nhiet do : "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F "));
  
    Firebase.setFloat(firebaseData, path + "/Do_am_khong_khi", h);
    Firebase.setFloat(firebaseData, path + "/Nhiet_do(do_C)", t);

    float moisture_percentage;
    int sensor_analog;
    
    sensor_analog = analogRead(sensor_pin);
    moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );

    Serial.print("Do am dat : ");
    Serial.print(moisture_percentage);
    Serial.print("%\n\n");

    Firebase.setFloat(firebaseData, path + "/Do_am_dat", moisture_percentage);

    if (moisture_percentage < threshold)
    {
      Serial.println("Do am dat nho hon 40%");
      Firebase.setString(firebaseData, path + "/Thong_bao", "Độ ẩm đất thấp");

      while(moisture_percentage < 65)
      {
        digitalWrite(pump, HIGH);
        Serial.println("Bat may bom");
        Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "BẬT");
        delay(1000);

        digitalWrite(pump, LOW);
        Serial.println("Tat may bom");
        Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "TẮT");
        
        sensor_analog = analogRead(sensor_pin);
        moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
        Firebase.setFloat(firebaseData, path + "/Do_am_dat", moisture_percentage);
        delay(1000);
      }    
    }
    else
    {
      digitalWrite(pump, LOW);
      Serial.println("Tat may bom");
      Serial.println("Do am dat on dinh");
      Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "TẮT");
      Firebase.setString(firebaseData, path + "/Thong_bao", "Độ ẩm đất ổn định");
    }   
}   