#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h> //https://github.com/arkhipenko/TaskScheduler

LTC4624 Mylcd; //initialize LCD

Scheduler runner;

void t1Callback();

Task t1(7, TASK_FOREVER, &t1Callback, &runner, true);

//------------------------------------------------------------ TASKS
void t1Callback() {
  Mylcd.lcdScan();  
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

  Mylcd.lcdInit();
  Mylcd.setDutyCycleLcd(255);

  Mylcd.lcdPrint(0, "D -");  
  Mylcd.lcdDot(0, 1);
  Mylcd.lcdDot(1, 0);
  Mylcd.lcdDot(2, 0);

  runner.startNow();  // set point-in-time for scheduling start
}

void loop() {
  runner.execute();
}
