#include <Arduino.h>

#define LMT84_PIN A0

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  lmt84_temperature();
}

int lmt84_temperature(){
  
  int temperature = analogRead(LMT84_PIN);
  float temperature_mV = ((float) temperature) / 1024.0 * 5000;
  Serial.println(temperature);
  Serial.println(temperature_mV);

  delay(1);  
}