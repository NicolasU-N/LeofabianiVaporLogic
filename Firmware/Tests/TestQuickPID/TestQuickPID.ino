/********************************************************
   PID Adaptive Tuning Example
   One of the benefits of the PID library is that you can
   change the tuning parameters at any time.  this can be
   helpful if we want the controller to be agressive at some
   times, and conservative at others.   in the example below
   we set the controller to use Conservative Tuning Parameters
   when we're near setpoint and more agressive Tuning
   Parameters when we're farther away.
 ********************************************************/

#include "QuickPID.h" //https://github.com/Dlloydev/QuickPID/blob/master/examples/PID_AdaptiveTunings/PID_AdaptiveTunings.ino
#include "max6675.h"
#include "PWM.h"

#define THERMODO    4
#define THERMOCS    5
#define THERMOCLK   6

MAX6675 thermocouple(THERMOCLK, THERMOCS, THERMODO);

//Define Variables we'll be connecting to
float Setpoint, Input, Output;

//Define the aggressive and conservative and POn Tuning Parameters
float aggKp = 4, aggKi = 0.2, aggKd = 1;
float consKp = 1, consKi = 0.05, consKd = 0.25;
float aggPOn =  1.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float consPOn = 0.0;  // proportional on Error to Measurement ratio (0.0-1.0)
float aggDOn =  1.0;  // derivative on Error to Measurement ratio (0.0-1.0)
float consDOn = 0.0;  // derivative on Error to Measurement ratio (0.0-1.0)

//Specify the links and initial tuning parameters
QuickPID myQuickPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, aggPOn, consDOn, QuickPID::DIRECT); // REVERSE

void setup()
{
  Serial.begin(115200);

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);
  setDutyPWMPD3(0);

  //initialize the variables we're linked to

  Setpoint = 150;

  //turn the PID on
  myQuickPID.SetMode(QuickPID::AUTOMATIC);
  myQuickPID.SetTunings(consKp, consKi, consKd, consPOn, consDOn);
  Serial.println("TUNING CONS");
}

void loop()
{
  Input = (float)thermocouple.readCelsius();
  Serial.println(Input);

  float gap = abs(Setpoint - Input); //distance away from setpoint
  Serial.print("GAP = ");
  Serial.println(gap);
  //if (gap < 10) { //we're close to setpoint, use conservative tuning parameters

  //} else {
  //we're far from setpoint, use aggressive tuning parameters
  //myQuickPID.SetTunings(aggKp, aggKi, aggKd, aggPOn, aggDOn);
  //Serial.println("TUNING AGG");
  //}
  myQuickPID.Compute();
  setDutyPWMPD3(Output);
  delay(400);
}
