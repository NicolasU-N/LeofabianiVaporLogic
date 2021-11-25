#include <EEPROM.h>
#include <Wire.h>
#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h>

LTC4624 Mylcd; //initialize LCD

Scheduler runner;

void t1Callback();

Task t1(1000, TASK_FOREVER, &t1Callback, &runner, true);

uint8_t dutyCycleLcd = 0;

//------------------------------------------------------------ TASKS
void t1Callback() {
  //Serial.println("Hola");
}

void setup() {
  Serial.begin(115200);

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);
  setDutyPWMPD3(0);

  Mylcd.sevenSegSetup();

  Mylcd.setDutyCycleLcd(255);


  Mylcd.setDecimalPoint(true, 1);
  Mylcd.setDecimalPoint(false, 0);
  
  Mylcd.setNumber(141);

  //Mylcd.setDigit(0, Mylcd.getSevenSegFont(13)); // A


  runner.startNow();  // set point-in-time for scheduling start

}

void loop() {


  runner.execute();

  Mylcd.displayHold(900000000);


  // Se enciende con 0 el segmento
  // Se apaga con 1 el segmento

  /*
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
  */
}
