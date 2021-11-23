#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library

#define DISPLAY_A1 11
#define DISPLAY_A2 9
#define DISPLAY_A3 10

// Set i2c address
PCF8574 pcf8574(0x20); //&Wire,

void setup() {
  Serial.begin(115200);

  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.pinMode(P4, OUTPUT);
  pcf8574.pinMode(P5, OUTPUT);
  pcf8574.pinMode(P6, OUTPUT);
  pcf8574.pinMode(P7, OUTPUT);

  Wire.begin();
  if (!pcf8574.begin()) {
    Serial.println("ERROR PCF8574");
  }
}

void loop() {
  // Se enciende con 0 el segmento
  // Se apaga con 1 el segmento

  pcf8574.digitalWrite(P0, 0);
  pcf8574.digitalWrite(P1, 0);
  pcf8574.digitalWrite(P2, 0);
  pcf8574.digitalWrite(P3, 0);
  pcf8574.digitalWrite(P4, 0);
  pcf8574.digitalWrite(P5, 0);
  pcf8574.digitalWrite(P6, 1);
  pcf8574.digitalWrite(P7, 1);
  analogWrite(DISPLAY_A1, 255);
  analogWrite(DISPLAY_A2, 0);
  analogWrite(DISPLAY_A3, 0);
  delay(4);

  pcf8574.digitalWrite(P0, 0);
  pcf8574.digitalWrite(P1, 0);
  pcf8574.digitalWrite(P2, 0);
  pcf8574.digitalWrite(P3, 0);
  pcf8574.digitalWrite(P4, 0);
  pcf8574.digitalWrite(P5, 0);
  pcf8574.digitalWrite(P6, 1);
  pcf8574.digitalWrite(P7, 1);

  analogWrite(DISPLAY_A1, 0);
  analogWrite(DISPLAY_A2, 255);
  analogWrite(DISPLAY_A3, 0);
  delay(4);


  pcf8574.digitalWrite(P0, 0);
  pcf8574.digitalWrite(P1, 0);
  pcf8574.digitalWrite(P2, 0);
  pcf8574.digitalWrite(P3, 0);
  pcf8574.digitalWrite(P4, 0);
  pcf8574.digitalWrite(P5, 0);
  pcf8574.digitalWrite(P6, 1);
  pcf8574.digitalWrite(P7, 1);

  analogWrite(DISPLAY_A1, 0);
  analogWrite(DISPLAY_A2, 0);
  analogWrite(DISPLAY_A3, 255);
  delay(4);
}
