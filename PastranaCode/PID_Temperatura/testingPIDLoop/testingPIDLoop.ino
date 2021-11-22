#include <PID_v1.h>
#include "max6675.h"

//THERMO VARIABLES
#define thermoDO 4
#define thermoCS 5
#define thermoCLK 6

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//PID VARIABLES
#define OUT_PWR 3
double Setpoint, Input, Output;

// PID TUNNING
double Kp=1.5, Ki=0.8, Kd=0.2;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Setpoint = 150;
  myPID.SetMode(AUTOMATIC); //Turn ON PID

  
}

void loop() {
  // put your main code here, to run repeatedly:
  pidLoop();
  delay(400); //400 for 3.3V  200 for 5.0 
}


float tempStatusCelcius(){
    float Celcius = thermocouple.readCelsius();
    Serial.print("Celcius:");
    Serial.println(Celcius);
    return Celcius;
}

float tempStatusFarenheit(){
    float Farenheit = thermocouple.readFahrenheit();
    Serial.print("Farenheit:");
    Serial.println(Farenheit);
    return Farenheit;
}

void pidLoop(){
  Input = (double)tempStatusCelcius();
  myPID.Compute();
  Serial.print("Output signal: ");
  Serial.println(Output);
  analogWrite(OUT_PWR, Output);
  }
