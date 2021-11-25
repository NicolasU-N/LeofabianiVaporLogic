#include <EEPROM.h>
#include <Wire.h>
#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h>
#include "max6675.h"

#define LEDGREEN  2
#define LEDBLUE   A0
#define LEDRED    7

#define THERMODO    4
#define THERMOCS    5
#define THERMOCLK   6

MAX6675 thermocouple(THERMOCLK, THERMOCS, THERMODO);

LTC4624 Mylcd; //initialize LCD

Scheduler runner;

void t1Callback();
void t2Callback();

Task t1(2000, TASK_FOREVER, &t1Callback, &runner, true);
Task t2(1000, TASK_FOREVER, &t2Callback, &runner, true);

uint8_t dutyCycleLcd = 0;

boolean togglelcdmode = true;
boolean toggleledgreen = true;

//------------------------------------------------------------ TASKS
void t1Callback() {
  if (togglelcdmode)
  {
    Mylcd.setDutyCycleLcd(255);
    Mylcd.setNumber((int)thermocouple.readCelsius());
    togglelcdmode = !togglelcdmode;
  }
  else
  {
    Mylcd.setDutyCycleLcd(127);
    Mylcd.setNumber((int)thermocouple.readFahrenheit());
    
    //Mylcd.setDigit(0, Mylcd.getSevenSegFont(11)); // A
    //Mylcd.setDigit(1, Mylcd.getSevenSegFont(12)); // B
    //Mylcd.setDigit(2, Mylcd.getSevenSegFont(16)); // C
    togglelcdmode = !togglelcdmode;
  }
}

void t2Callback() {
  if (toggleledgreen)
  {
    digitalWrite(LEDGREEN, HIGH);   // set the LED on
    toggleledgreen = !toggleledgreen;
  }
  else
  {
    digitalWrite(LEDGREEN, LOW);    // set the LED off
    toggleledgreen = !toggleledgreen;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE, OUTPUT);
  pinMode(LEDRED, OUTPUT);

  digitalWrite(LEDGREEN,  1); //OFF IN 1
  digitalWrite(LEDBLUE,  1);
  digitalWrite(LEDRED,  1);

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);
  setDutyPWMPD3(0);

  Mylcd.sevenSegSetup();
  Mylcd.setDutyCycleLcd(255);

  //Mylcd.setDecimalPoint(false, 1);
  //Mylcd.setDecimalPoint(false, 0);

  Mylcd.setNumber(141);

  //Mylcd.setDigit(0, Mylcd.getSevenSegFont(13)); // A


  runner.startNow();  // set point-in-time for scheduling start

  //t3.disable();
}

void loop() {


  runner.execute();

  Mylcd.displayHold(20);


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
