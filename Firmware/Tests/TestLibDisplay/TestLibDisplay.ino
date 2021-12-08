#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <Wire.h>
#include "PWM.h"
#include "LTC4624.h"
#include <TaskScheduler.h> //https://github.com/arkhipenko/TaskScheduler
#include "max6675.h"
#include "OneButton.h"
#include "QuickPID.h"      //https://github.com/Dlloydev/QuickPID/blob/master/examples/PID_AdaptiveTunings/PID_AdaptiveTunings.ino

#define LEDGREEN    2
#define LEDBLUE     A0
#define LEDRED      7

#define THERMODO    4
#define THERMOCS    5
#define THERMOCLK   6

#define BTN1        A1
#define BTN2        A3
#define UTEMP       A2

// ----------------------------------------------- THERMAL_PROTECTION
#define WATCH_TEMP_PERIOD 20  // Seconds
#define WATCH_TEMP_INCREASE 2 // Degrees Celsius 
// -----------------------------------------------

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

void t5Callback();
void t6Callback();
void t7Callback();
void t8Callback();

Task t0(7,      TASK_FOREVER, &t0Callback, &runner, true);    // 142.85Hz lcdScan
Task t1(2000,   TASK_FOREVER, &t1Callback, &runner, true);    // Presetmode Temp - Setpoint
Task t2(1000,   TASK_FOREVER, &t2Callback, &runner, true);    // Blink of led green 1Hz
Task t3(400,    TASK_FOREVER, &t3Callback, &runner, true);    // Read thermocouple temperature every 400 ms
Task t4(60000,  TASK_FOREVER, &t4Callback, &runner, true);    // 300000 Read internal temperature

//PID and preheat
Task t5(401, TASK_FOREVER, &t5Callback, &runner, true);

//Error
Task t6(202, 18, &t6Callback, &runner, true);                 // blink SOS
Task t7(1001, TASK_FOREVER, &t7Callback, &runner, true);      // Error check
Task t8(WATCH_TEMP_PERIOD * TASK_SECOND, TASK_FOREVER, &t8Callback, &runner, true);      // THERMAL_PROTECTION

///////////////////////////////////////////////////////// PID
float Setpoint = 150; //In degrees celsius
float readTemp = 0;   //Celcius
float Output;

//Define the aggressive and conservative and POn Tuning Parameters
float aggKp = 4, aggKi = 0.2, aggKd = 1;
float consKp = 1, consKi = 0.05, consKd = 0.25;
float aggPOn =  1.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float consPOn = 0.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float aggDOn =  1.0;  // derivative on Error to Measurement ratio (0.0-1.0)
float consDOn = 0.0;  // derivative on Error to Measurement ratio (0.0-1.0)

//Specify the links and initial tuning parameters
QuickPID myQuickPID(&readTemp, &Output, &Setpoint, consKp, consKi, consKd, aggPOn, consDOn, QuickPID::DIRECT);
/////////////////////////////////////////////////////////

void (*resetFunc)(void) = 0;

//#######################################################
#define CRITICAL_TEMP 70

boolean toggleLcdMode = true;
boolean toggleLedGreen = true;

String  tempThermocouple;   //Celcius or Fahrenheit
boolean flagTemp = true;    //true  -> Celcius

float utemp = 0;

float antgap = 0;

boolean flagErrorThermocouple = false;  //false -> no error
boolean flagErrorUTemp = false;
boolean flagErrorHeater = false;

uint8_t dutyDisplay = 255;

#define NORMALMODE 0
#define INTOMENU 1
#define INTOPRE 2
#define INTODEGREES 3
#define INTOPWMLCD 4

uint8_t STATE = NORMALMODE;

#define PREHEATING 0
#define HEATINGPID 1

uint8_t HEATERSTATE = PREHEATING;

uint8_t counterclick1 = 0;
uint8_t counterclick2 = 0;
uint8_t maxIndex = 0;

const char *menuTitles[] = {
  "PrE",  //Presets
  "DE6",  //Change measurement °C <-> °F
  "Br1",  //Brightness adjustment
  "rE5",  //Reset factory settings
};

const char *preTitles[] = {
  "150",
  "200",
  "250",
  "300",
  "350",
};

const char *degTitles[] = {
  " C ",
  " F ",
};

const char *brigTitles[] = {
  "32 ",
  "64 ",
  "127",
  "178",
  "216",
  "255",
};

//#######################################################

//---------------------------------------------------------------------------------------- TASKS
/*
   @brief lcdScan
*/
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


/*
   @brief blink of led 1Hz
*/
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

  if (!isnan(thermocouple.readCelsius())) {
    readTemp = thermocouple.readCelsius();
    tempThermocouple = flagTemp ? (String)thermocouple.readCelsius() : (String)thermocouple.readFahrenheit();
    Serial.println(tempThermocouple);
    //t5.enable(); //PID
    flagErrorThermocouple = false;
  } else {
    flagErrorThermocouple = true;
  }


}


/*
   @brief Read internal temperature
*/
void t4Callback() {
  //Serial.print("Internal temperature: ");

  float readUTemp = (analogRead(UTEMP) / 1023.0) * 5.0 * 1000; //convert mV //////////////////////////////////////////////////////////////////////////////////////////////// 3.3
  utemp = -0.000010541 * pow(readUTemp, 2) - 0.1636 * (readUTemp) + 180.3525;
  Serial.print(F("readUTemp = "));
  Serial.print(readUTemp);
  Serial.print(F("\t utemp = "));
  Serial.println(utemp);

  if (utemp >= CRITICAL_TEMP) { //Critical temperature
    flagErrorUTemp = true;
  } else {
    flagErrorUTemp = false;
  }

  //Serial.println(chipTemp.deciCelsius() / 10.0, 1); //     chipTemp.deciCelsius() * gain / 10 + offset - gain * CALIBRATION_TEMP
  //Serial.println(); //chipTemp.deciCelsius() / 10.0, 1

  /*
    if ((chipTemp.deciCelsius() / 10.0) > CRITICAL_TEMP) { //Critical temperature
      t5.disable(); // Disable task PID and off output
      setDutyPWMPD3(0);
    } else {

    }
  */

}


/*
   @brief Task in charge of executing the pid
*/
void t5Callback() {
  Serial.println(F("---------------- PID ----------------"));
  float gap = abs(Setpoint - readTemp); //distance away from setpoint
  Serial.println(gap);

  if (gap < 10) { //we're close to setpoint, use conservative tuning parameters
    myQuickPID.SetTunings(consKp, consKi, consKd, consPOn, consDOn);
    HEATERSTATE = HEATINGPID;
  } else {
    //we're far from setpoint, use aggressive tuning parameters
    myQuickPID.SetTunings(aggKp, aggKi, aggKd, aggPOn, aggDOn);
    HEATERSTATE = PREHEATING;//set state preheat
  }

  switch (HEATERSTATE) {
    case HEATINGPID:
      myQuickPID.Compute();
      setDutyPWMPD3((int)Output);
      Output > 25 ? digitalWrite(LEDBLUE, 0) : digitalWrite(LEDBLUE, 1);
      t8.disable();
      break;
    case PREHEATING:
      setDutyPWMPD3(63); // 25% output
      t8.enableIfNot(); // enable
      break;
  }
}


/*
   @brief The red led is blinked in morse SOS
*/
void t6Callback() {
  t6.getRunCounter() > 5 and t6.getRunCounter() < 13 ? t6.setInterval(502) : t6.setInterval(202);

  if ( t6.getRunCounter() & 1 ) {
    digitalWrite(LEDRED, LOW);
  }
  else {
    digitalWrite(LEDRED, HIGH);
  }

  if ( t6.isLastIteration() ) {
    t6.restartDelayed( 1 * TASK_SECOND );
    digitalWrite(LEDRED, HIGH);
  }
}


/*
   @brief Check for an error in the system
*/
void t7Callback() {

  if (flagErrorThermocouple || flagErrorUTemp || flagErrorHeater) {
    setDutyPWMPD3(0);
    t1.disable(); // Disable Presetmode Temp - Setpoint
    t6.enableIfNot();  // Enable blink SOS
  } else {
    t1.enableIfNot();  // Enable Presetmode Temp - Setpoint
    t6.disable(); // Disable blink SOS
  }

  if (flagErrorThermocouple) {
    Mylcd.lcdPrint(0, "Er1"); // ER1 -> Does not detect thermocouple
  }


  if (flagErrorUTemp) {
    Mylcd.lcdPrint(0, "Er2"); // ER2 -> Microcontroller temperature is higher than critical temperature
  }

  if (flagErrorHeater) {
    Mylcd.lcdPrint(0, "Er3"); // ER3 -> There is a problem with the heater
  }

}


/*
   @brief THERMAL_PROTECTION_INCREASE
*/
void t8Callback() {

  if (!t8.isFirstIteration()) {
    float gap1 = abs(Setpoint - readTemp); //distance away from setpoint

    Serial.print(F("gap1 = "));
    Serial.print(gap1);
    Serial.print(F("\t antgap = "));
    Serial.println(antgap);

    if (gap1 >= antgap + WATCH_TEMP_INCREASE) {
      flagErrorHeater = false;
    } else {
      flagErrorHeater = true;
    }

    antgap = gap1;
  }

}

//----------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------- BTN 1 callback functions

/*
   @brief This function will be called when the button1 was pressed 1 time.
*/
void click1() {
  if (STATE != NORMALMODE) {

    counterclick1++;

    if (counterclick1 == maxIndex) {
      counterclick1 = 0;
    }

    Serial.print(F("Max index click 1: "));
    Serial.println(maxIndex);
    Serial.print(F("Current index"));
    Serial.println(counterclick1);

    switch (STATE) {
      case INTOMENU:
        Mylcd.lcdPrint(0, menuTitles[counterclick1]);
        break;
      case INTOPRE:
        Mylcd.lcdPrint(0, preTitles[counterclick1]);
        break;
      case INTODEGREES:
        Mylcd.lcdPrint(0, degTitles[counterclick1]);
        break;
      case INTOPWMLCD:
        Mylcd.lcdPrint(0, brigTitles[counterclick1]);
        break;
    }
    counterclick2 = counterclick1;
  }
}

/*
   @brief This function will be called once, when the button1 is pressed for a long time.
*/
void longPressStart1() {
  Serial.println("Button 1 longPress start");

  switch (STATE) {
    case INTOMENU:
      if (menuTitles[counterclick1] == "PrE" ) {
        maxIndex = *(&preTitles + 1) - preTitles; //length of array
        STATE = INTOPRE;
        Mylcd.lcdPrint(0, preTitles[0]);
        counterclick1 = 0;
        counterclick2 = 0;
        Serial.println(F("PRE"));
      }
      if (menuTitles[counterclick1] == "DE6" ) {
        maxIndex = *(&degTitles + 1) - degTitles; //length of array
        STATE = INTODEGREES;
        Mylcd.lcdPrint(0, degTitles[0]);
        counterclick1 = 0;
        counterclick2 = 0;
        Serial.println(F("DEG"));
      }
      if (menuTitles[counterclick1] == "Br1" ) {
        maxIndex = *(&brigTitles + 1) - brigTitles; //length of array
        STATE = INTOPWMLCD;
        Mylcd.lcdPrint(0, brigTitles[0]);
        counterclick1 = 0;
        counterclick2 = 0;
        Serial.println(F("BRI"));
      }
      if (menuTitles[counterclick1] == "rE5" ) {
        maxIndex = 0; //length of array
        STATE = NORMALMODE;
        Mylcd.lcdPrint(0, "---");
        counterclick1 = 0;
        counterclick2 = 0;
        EEPROM.put(1, 150.0); // Setpoint
        EEPROM.put(0, 1); // °C
        EEPROM.put(1 + sizeof(float), 255); // brightness
        Serial.println(F("RESET"));
        resetFunc();
      }
      break;
    case INTOPRE:

      Setpoint = atof(preTitles[counterclick1]);
      Serial.println(Setpoint);

      EEPROM.put(1, Setpoint); // Setpoint

      t1.enableIfNot();
      counterclick1 = 0;
      counterclick2 = 0;
      STATE = NORMALMODE;
      break;
    case INTODEGREES:

      if (degTitles[counterclick1] == " C " ) {
        Serial.println("C");
        flagTemp = true;
        EEPROM.put(0, 1); // °C
      }
      if (degTitles[counterclick1] == " F " ) {
        Serial.println("F");
        flagTemp = false;
        EEPROM.put(0, 0); // °F
      }

      t1.enableIfNot();
      counterclick1 = 0;
      counterclick2 = 0;
      STATE = NORMALMODE;
      break;
    case INTOPWMLCD:

      Mylcd.setDutyCycleLcd(atoi(brigTitles[counterclick1]));
      Serial.println(atoi(brigTitles[counterclick1]));

      EEPROM.put(1 + sizeof(float), atoi(brigTitles[counterclick1])); // brightness

      t1.enableIfNot();
      counterclick1 = 0;
      counterclick2 = 0;
      STATE = NORMALMODE;
      break;
  }
}

//---------------------------------------------------------------------------------------- BTN 2 callback functions

void click2() {
  if (STATE != NORMALMODE) {

    if (counterclick2 <= 0) {
      counterclick2 = maxIndex;
    }

    counterclick2--;

    Serial.print(F("Max index click 2: "));
    Serial.println(maxIndex);
    Serial.print(F("Current index"));
    Serial.println(counterclick2);

    switch (STATE) {
      case INTOMENU:
        Mylcd.lcdPrint(0, menuTitles[counterclick2]);
        break;
      case INTOPRE:
        Mylcd.lcdPrint(0, preTitles[counterclick2]);
        break;
      case INTODEGREES:
        Mylcd.lcdPrint(0, degTitles[counterclick2]);
        break;
      case INTOPWMLCD:
        Mylcd.lcdPrint(0, brigTitles[counterclick2]);
        break;
    }
    counterclick1 = counterclick2;
  }
}

void longPress2() {

  switch (STATE) {
    case NORMALMODE:
      if (button1.isLongPressed() && button2.isLongPressed()) {
        Serial.println(F("Enter Menu"));
        t1.disable(); //Disable toggle
        //CHANGE STATE
        Mylcd.lcdPrint(0, menuTitles[0]);
        maxIndex = *(&menuTitles + 1) - menuTitles; //length of array
        STATE = INTOMENU;
      }
      break;
  }

}

void setup() {
  Serial.begin(115200);

  pinMode(LEDGREEN, OUTPUT);
  pinMode(LEDBLUE,  OUTPUT);
  pinMode(LEDRED,   OUTPUT);
  pinMode(UTEMP,   INPUT);

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

  //Load last configuration
  Serial.print(F("Read Address 0 (°C/°F): "));
  if (EEPROM.read(0)) { //Read temperature flag status    (Address 0)
    flagTemp = true;
    Serial.println(F("true"));
  } else {
    flagTemp = false;
    Serial.println(F("false"));
  }

  Serial.print(F("Read Address 1 setpoint: "));
  EEPROM.get(1, Setpoint);
  Serial.println(Setpoint);

  Serial.print(F("Read Address 2 brightness: "));
  EEPROM.get(1 + sizeof(float), dutyDisplay);
  Serial.println(dutyDisplay);


  // Init lcd
  Mylcd.lcdInit();
  Mylcd.setDutyCycleLcd(dutyDisplay);
  Mylcd.lcdPrint(0, "888");
  Mylcd.lcdDot(0, 0);
  Mylcd.lcdDot(1, 0);
  Mylcd.lcdDot(2, 0);

  //Init Btns
  button1.attachClick(click1);
  button1.attachLongPressStart(longPressStart1);
  button1.attachDuringLongPress(longPress2);
  button2.attachClick(click2);
  button2.attachLongPressStart(longPressStart1);
  button2.attachDuringLongPress(longPress2);

  //turn the PID on
  myQuickPID.SetMode(QuickPID::AUTOMATIC);

  runner.startNow();  // set point-in-time for scheduling start
  t6.disable(); // blink SOS
  //t8.disable(); // THERMAL_PROTECTION
}


void loop() {
  runner.execute();

  // keep watching the push buttons:
  button1.tick();
  button2.tick();
}
