#include <Wire.h>
#include "PWM.h"
//#include "LTC4624.h"
#include <TaskScheduler.h>

//LTC4624 Mylcd; //initialize LCD

Scheduler runner;

void t1Callback();

Task t1(50, TASK_FOREVER, &t1Callback, &runner, true);

//------------------------------------------------------------ TASKS
void t1Callback() {
  //Mylcd.lcdScan();
  Serial.println("Hola");
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

  //Mylcd.dutyCycleLcd = 255;

  //Mylcd.lcdPrint(0, "123");
  //Mylcd.lcdDot(0, 1);
  //Mylcd.lcdDot(1, 1);
  //Mylcd.lcdDot(2, 1);

  runner.startNow();  // set point-in-time for scheduling start

}

void loop() {
  

  runner.execute();


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
