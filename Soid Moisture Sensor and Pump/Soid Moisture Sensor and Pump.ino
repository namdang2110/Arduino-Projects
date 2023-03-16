#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "FirebaseESP8266.h"
#include "DHT.h"

#define DHTPIN 4             // Khai báo chân DHT22 là chân D2 trên esp8266
#define DHTTYPE DHT22        // Khai báo loại DHT sử dụng là DHT22

#define WIFI_SSID "CTD.TNUT"
#define WIFI_PASSWORD "123456789"

#define FIREBASE_HOST "iot-plant-174a3-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "IviXf2ES24Ev7stjRCQVbbph5wdnvwcq6yVxQTOh"

FirebaseData firebaseData;
//WiFiClient client;

String path = "/";
FirebaseJson json;

int sensor_pin = A0; 
int output_value ;
const int pump = 5;         // Khai báo chân của Relay là chân D1 của esp8266
const int threshold = 35;   // Khai báo ngưỡng độ ẩm để so sánh
DHT dht(DHTPIN, DHTTYPE);
int val=0;

void setup() {
  Serial.begin(115200);
  Serial.println("Dang doc cam bien...");
  pinMode(pump, OUTPUT);
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

  delay(2000);
  }

void loop() {

    output_value= analogRead(sensor_pin);
    output_value = map(output_value,550,0,0,100);
    Serial.print("Do am dat : ");
    Serial.print(output_value);
    Serial.println("%");

    Firebase.setInt(firebaseData, path + "/Do_am_dat", output_value);

    float h = dht.readHumidity();           // Đọc độ ẩm trên DHT22
    float t = dht.readTemperature();        // Đọc nhiệt độ là °C trên DHT22
    float f = dht.readTemperature(true);    // Đọc nhiệt độ là °F trên DHT22

    if (isnan(h) || isnan(t) || isnan(f)) 
    {
      Serial.println(F("Loi khi doc module DHT22 !"));
      return;
    }

    float hif = dht.computeHeatIndex(f, h);         // Đọc chỉ số nhiệt độ trên DHT22
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
  
    Firebase.setInt(firebaseData, path + "/Do_am_khong_khi", h);
    Firebase.setInt(firebaseData, path + "/Nhiet_do(do_C)", t);

    int check_value = output_value;

    if (output_value < threshold)     // Nếu độ ẩm nhỏ hơn ngưỡng đặt trước thì bật máy bơm
    {
      digitalWrite(pump, HIGH);
      Serial.println("Bat may bom");
      Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "BẬT");
      delay(2000);    // Chạy máy bơm trong 1 giây

      digitalWrite(pump, LOW);
      Serial.println("Tat may bom");
      Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "TẮT");
      delay(8000);    // Tắt máy bơm và chờ 9 giây

      output_value= analogRead(sensor_pin);
      output_value = map(output_value,550,0,0,100);     // Đọc module ẩm đất lần 2 để kiểm tra hỏng hóc

      if (check_value == output_value)      // Kiểm tra module ẩm đất có hỏng hay không
      {
        Serial.println("CANH BAO : MODULE DO AM DAT HONG ! ");
        Firebase.setString(firebaseData, path + "/CANH_BAO", "CẢM BIẾN ĐỘ ẨM ĐẤT HOẶC MÁY BƠM BỊ HỎNG !");
      }
    }
    else
    {
      digitalWrite(pump, LOW);
      Serial.println("Da du do am. Tat may bom");
      Firebase.setString(firebaseData, path + "/Trang_thai_may_bom", "TẮT");
      Firebase.setString(firebaseData, path + "/CANH_BAO", " ");
    }
    delay(50000);   // Chờ 10 giây và bắt đầu lại vòng lặp
  }