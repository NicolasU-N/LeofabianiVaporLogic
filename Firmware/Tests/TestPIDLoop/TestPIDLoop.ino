#include <PID_v1.h>
#include "max6675.h"
#include "PWM.h"

//THERMO VARIABLES
#define thermoDO 4
#define thermoCS 5
#define thermoCLK 6

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//PID VARIABLES
#define OUT_PWR 3
double Setpoint, Input, Output;

// PID TUNNING
double Kp = 1.0, Ki = 0 , Kd = 0.4; //0.15    0.45
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Setpoint = 150;
  myPID.SetMode(AUTOMATIC); //Turn ON PID

  // Init PWM PINS
  PWMInit();
  PWMOn();
  setDutyPWMPB1(0);
  setDutyPWMPB2(0);
  setDutyPWMPB3(0);
  setDutyPWMPD3(0);


}

void loop() {
  // put your main code here, to run repeatedly:
  pidLoop();
  delay(400); //400 for 3.3V  200 for 5.0
}


float tempStatusCelcius() {
  float Celcius = thermocouple.readCelsius();
  //Serial.print("Celcius: ");
  //Serial.println(Celcius);
  return Celcius;
}

float tempStatusFarenheit() {
  float Farenheit = thermocouple.readFahrenheit();
  Serial.print("Farenheit:");
  Serial.println(Farenheit);
  return Farenheit;
}

void pidLoop() {
  Input = (double)tempStatusCelcius();
  myPID.Compute();
  //Serial.print("Outputsignal: ");
  //Serial.println(Output);
  Serial.print("Setpoint:");  Serial.print(Setpoint);  Serial.print(",");
  Serial.print("Input:");     Serial.print(Input);     Serial.print(",");
  Serial.print("Output:");    Serial.print(Output);    Serial.println(",");
  //analogWrite(OUT_PWR, Output);
  setDutyPWMPD3((int)Output);

}
