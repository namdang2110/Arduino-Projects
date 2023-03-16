#include <ESP8266WiFi.h>

#define sensor A0
#define waterPump D3

void setup() {
  Serial.begin(9600);
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);

  timer.setInterval(100L, soilMoistureSensor);
}

//Get the button value
BLYNK_WRITE(V1) {
  Relay = param.asInt();

  if (Relay == 1) 
  {
    digitalWrite(waterPump, LOW);
  } else {
    digitalWrite(waterPump, HIGH);
  }
}

//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(sensor);
  value = map(value, 0, 1024, 0, 100);
  value = (value - 100) * -1;

  Blynk.virtualWrite(V0, value);
  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print(" ");

}

void loop() {
  Blynk.run();//Run the Blynk library
  timer.run();//Run the Blynk timer
}