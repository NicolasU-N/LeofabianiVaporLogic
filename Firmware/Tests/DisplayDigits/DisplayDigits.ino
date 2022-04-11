#include <Wire.h>
#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library
#include "PWM.h"

#define ADDR_PCF        0x20 //0x38

#define SEG_H_PIN       P7
#define SEG_A_PIN       P0
#define SEG_B_PIN       P1
#define SEG_C_PIN       P2
#define SEG_D_PIN       P3
#define SEG_E_PIN       P4
#define SEG_F_PIN       P5
#define SEG_G_PIN       P6

static char digit = 0;

// Set i2c address
PCF8574 pcf8574(ADDR_PCF); //&Wire,

void setup() {
  //Serial.begin(115200);

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);

  //display
  pcf8574.pinMode(SEG_A_PIN, OUTPUT);
  pcf8574.pinMode(SEG_B_PIN, OUTPUT);
  pcf8574.pinMode(SEG_C_PIN, OUTPUT);
  pcf8574.pinMode(SEG_D_PIN, OUTPUT);
  pcf8574.pinMode(SEG_E_PIN, OUTPUT);
  pcf8574.pinMode(SEG_F_PIN, OUTPUT);
  pcf8574.pinMode(SEG_G_PIN, OUTPUT);
  pcf8574.pinMode(SEG_H_PIN, OUTPUT);

  if (!pcf8574.begin()) {
    Serial.println("ERROR PCF8574");
  }
}

void loop() {

  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0); //turn off all digits
  
  pcf8574.digitalWrite(SEG_H_PIN, 0); // On
  pcf8574.digitalWrite(SEG_A_PIN, 0);
  pcf8574.digitalWrite(SEG_B_PIN, 0);
  pcf8574.digitalWrite(SEG_C_PIN, 0);
  pcf8574.digitalWrite(SEG_D_PIN, 0);
  pcf8574.digitalWrite(SEG_E_PIN, 0);
  pcf8574.digitalWrite(SEG_F_PIN, 0);
  pcf8574.digitalWrite(SEG_G_PIN, 0);

  switch (digit) {
    case 0:
      setDutyPWMPB3(255);
      break;
    case 1:
      setDutyPWMPB1(255);
      break;
    case 2:
      setDutyPWMPB2(255);
      break;
  }

  if (++digit > 2) digit = 0;
  delay(1000);
}
