#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Wire.h>
#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h> //https://github.com/arkhipenko/TaskScheduler
#include "max6675.h"
#include "OneButton.h"
#include "ChipTemp.h"
#include "QuickPID.h" //https://github.com/Dlloydev/QuickPID/blob/master/examples/PID_AdaptiveTunings/PID_AdaptiveTunings.ino

#define LEDGREEN    2
#define LEDBLUE     A0
#define LEDRED      7

#define THERMODO    4
#define THERMOCS    5
#define THERMOCLK   6

#define BTN1        A1
#define BTN2        A3

ChipTemp chipTemp; // Internal temperature sensor

// Setup a new OneButton on pin A1.
OneButton button1(BTN1, true);
// Setup a new OneButton on pin A2.
OneButton button2(BTN2, true);

MAX6675 thermocouple(THERMOCLK, THERMOCS, THERMODO);

LTC4624 Mylcd; //initialize LCD

Scheduler runner;

void t0Callback();
void t1Callback();
void t2Callback();
void t3Callback();
void t4Callback();

//void t5Callback();


Task t0(7,      TASK_FOREVER, &t0Callback, &runner, true);
Task t1(2000,   TASK_FOREVER, &t1Callback, &runner, true);  //Presetmode Temp - Setpoint
Task t2(1000,   TASK_FOREVER, &t2Callback, &runner, true);
Task t3(400,    TASK_FOREVER, &t3Callback, &runner, true);
Task t4(60000,  TASK_FOREVER, &t4Callback, &runner, true);  //Read internal temperature

//States
//Task t5(1, TASK_ONCE, &t5Callback, &runner, true);          //Enter Configuration mode

///////////////////////////////////////////////////////// PID
float Setpoint = 150; //In degrees celsius
float Input;
float Output;

//Define the aggressive and conservative and POn Tuning Parameters
float aggKp = 4, aggKi = 0.2, aggKd = 1;
float consKp = 1, consKi = 0.05, consKd = 0.25;
float aggPOn =  1.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float consPOn = 0.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float aggDOn =  1.0;  // derivative on Error to Measurement ratio (0.0-1.0)
float consDOn = 0.0;  // derivative on Error to Measurement ratio (0.0-1.0)

//Specify the links and initial tuning parameters
QuickPID myQuickPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, aggPOn, consDOn, QuickPID::DIRECT);
/////////////////////////////////////////////////////////

//#######################################################
#define CALIBRATION_TEMP 32

boolean toggleLcdMode = true;
boolean toggleLedGreen = true;

String  tempThermocouple;
float   readTemp;           //Celcius or Fahrenheit
boolean flagTemp = true;    //true -> Celcius

uint8_t dutyDisplay = 255;

#define NORMALMODE 0
#define INTOMENU 1
#define INTOPRE 2
#define INTODEGREES 3
#define INTOPWMLCD 4

uint8_t STATE = NORMALMODE;

const char menuTitles [] = {
  "PrE",  //Presets
  "DE6",  //Change measurement °C <-> °F
  "Br1",  //Brightness adjustment
}

const char PreTitles [] = {
  "150",
  "200",
  "250",
  "300",
  "350",
}

const char degTitles [] = {
  " C ",
  " F ",
}

const char brigTitles [] = {
  "64",
  "127",
  "178",
  "216",
  "255",
}



//#######################################################

//---------------------------------------------------------------------------------------- TASKS
void t0Callback() {
  Mylcd.lcdScan();
}

/*
   @brief Shows the setpoint and after two seconds shows the current temperature
*/
void t1Callback() {
  if (toggleLcdMode)
  {
    Mylcd.lcdPrint(0, tempThermocouple.c_str());
    toggleLcdMode = !toggleLcdMode;
  }
  else
  {
    String strSetpoint = (String)Setpoint;
    Mylcd.lcdPrint(0, strSetpoint.c_str());
    toggleLcdMode = !toggleLcdMode;
  }
}

void t2Callback() {
  if (toggleLedGreen)
  {
    digitalWrite(LEDGREEN, HIGH);   // set the LED on
    toggleLedGreen = !toggleLedGreen;
  }
  else
  {
    digitalWrite(LEDGREEN, LOW);    // set the LED off
    toggleLedGreen = !toggleLedGreen;
  }
}

/*
   @brief Read thermocouple temperature every 400 ms
*/
void t3Callback() {
  readTemp = flagTemp ? thermocouple.readCelsius() : thermocouple.readFahrenheit();
  tempThermocouple = (String)readTemp;
  Serial.println(tempThermocouple);
}

/*
   @brief Read internal temperature
*/
void t4Callback() {
  Serial.print("New offset at CALIBRATION_TEMP degrees: ");
  Serial.println(chipTemp.deciCelsius() * gain / 10 + offset - gain * CALIBRATION_TEMP);
}

/*
   @brief Read internal temperature
*/
//void t5Callback() {
//  Serial.print("---------------- MENU ----------------");

//}
//----------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------- BTN 1 callback functions

/*
   @brief This function will be called when the button1 was pressed 1 time.
*/
void click1() {
  Serial.println("Button 1 click.");
}

/*
   @brief This function will be called once, when the button1 is pressed for a long time.
*/
void longPressStart1() {
  Serial.println("Button 1 longPress start");

  switch (STATE) {
    case NORMALMODE:

      if (button1.isLongPressed() && button2.isLongPressed()) {
        Serial.println("Enter Menu");
        t1.disable(); //Disable toggle
        //CHANGE STATE
      }

      break;
    case :
      break;
    case :
      break;
    case :
      break;
  }



}

//---------------------------------------------------------------------------------------- BTN 2 callback functions

void click2() {
  Serial.println("Button 2 click.");
}

void longPressStart2() {
  Serial.println("Button 2 longPress start");

  switch (STATE) {
    case NORMALMODE:
      break;
    case :
      break;
    case :
      break;
    case :
      break;
  }

  if (button1.isLongPressed() && button2.isLongPressed()) {
    Serial.println("Enter Menu");
  }

}

void setup() {
  Serial.begin(115200);

  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE,  OUTPUT);
  pinMode(LEDRED,   OUTPUT);

  digitalWrite(LEDGREEN,  1); //OFF IN 1
  digitalWrite(LEDBLUE,   1);
  digitalWrite(LEDRED,    1);

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);
  setDutyPWMPD3(0);

  // Init lcd
  Mylcd.lcdInit();
  Mylcd.setDutyCycleLcd(dutyDisplay);
  Mylcd.lcdPrint(0, "AAA");
  Mylcd.lcdDot(0, 0);
  Mylcd.lcdDot(1, 0);
  Mylcd.lcdDot(2, 0);

  //Init Btns
  button1.attachClick(click1);
  button1.attachLongPressStart(longPressStart1);
  button2.attachClick(click2);
  button2.attachLongPressStart(longPressStart2);

  /*
    if (button1.isLongPressed() && button2.isLongPressed()) {
    Serial.println("Enter Menu");
    }
  */

  //turn the PID on
  //myQuickPID.SetMode(QuickPID::AUTOMATIC);

  runner.startNow();  // set point-in-time for scheduling start
  //t5.disable();
}


void loop() {
  runner.execute();

  // keep watching the push buttons:
  button1.tick();
  button2.tick();
}
