#include <EEPROM.h>
#include <Wire.h>
#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h> //https://github.com/arkhipenko/TaskScheduler
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

void t0Callback();
void t1Callback();
void t2Callback();

Task t0(7, TASK_FOREVER, &t0Callback, &runner, true);
Task t1(2000, TASK_FOREVER, &t1Callback, &runner, true);
Task t2(1000, TASK_FOREVER, &t2Callback, &runner, true);

boolean togglelcdmode = true;
boolean toggleledgreen = true;
String tempthermocouple;

//------------------------------------------------------------ TASKS
void t0Callback() {
  Mylcd.lcdScan();
}

void t1Callback() {
  if (togglelcdmode)
  {
    Mylcd.setDutyCycleLcd(255);
    tempthermocouple = (String)thermocouple.readCelsius();
    Mylcd.lcdPrint(0, tempthermocouple.c_str());
    togglelcdmode = !togglelcdmode;
  }
  else
  {
    Mylcd.setDutyCycleLcd(127);
    tempthermocouple = (String)thermocouple.readFahrenheit();
    Mylcd.lcdPrint(0, tempthermocouple.c_str());
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

  Mylcd.lcdInit();
  Mylcd.setDutyCycleLcd(255);

  Mylcd.lcdPrint(0, "D -");

  Mylcd.lcdDot(0, 0);
  Mylcd.lcdDot(1, 0);
  Mylcd.lcdDot(2, 0);

  runner.startNow();  // set point-in-time for scheduling start
}


void loop() {
  runner.execute();
}
