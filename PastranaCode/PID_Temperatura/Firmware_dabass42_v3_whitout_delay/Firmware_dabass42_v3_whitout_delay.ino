#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <YetAnotherPcInt.h>
#include "SingleEMAFilterLib.h"

SingleEMAFilter<float> singleEMAFilter(0.5);

#define ORANGE  4
#define GREEN   5 //The LED exchange was carried out, before the 4 was green

#define MOTOR   3
#define BTN1    6
#define BTN2    7
#define VUSB    A2
#define ISAMPLE A1


float vbat = 0;
float current = 0;
float current1 = 0;
//boolean flagStart = true;
boolean flagMvolt = false ;

unsigned long currentMillis6 = 0;
unsigned long previousMillis6 = 0;

void (*resetFunc)(void) = 0;

// --------- STATE
#define NORMAL 0
#define CHARGE 1
#define MVOLT  2

uint8_t stateOp = 0;

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000010; // for PWM frequency of 3921.16 Hz

  pinMode(ORANGE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(MOTOR, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(ISAMPLE, INPUT);
  pinMode(VUSB, INPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  digitalWrite(GREEN, LOW);
  digitalWrite(ORANGE, LOW);
  
  Serial.begin(9600);
}

void loop() {

  switch (stateOp) {
    case NORMAL:
      ADCSRA |= (1 << ADEN);
      PcInt::attachInterrupt(VUSB, measureVolClick, CHANGE);//attaching a interrupt to pin d7
      //btn1.tick();

      //Serial.print("btn1->");
      //Serial.println(digitalRead(BTN1));
      //Serial.print("btn2->");
      //Serial.println(digitalRead(BTN2));

      vbat = (float)analogRead(A0) / 1023 * 12.0 * (30921.053 / 10000);
      //for (int i = 1; i <= 10; i++) {
      current = (float)analogRead(ISAMPLE); //+ current
      current *=  (12.0 / 1023);
      current /= 0.002 ;
      // Calcular filtro

      //}
      Serial.print("VBAT-> ");
      Serial.print(vbat);
      Serial.print("  | CURRENT-> ");
      Serial.println(current1);
      //current = current / 10;
      singleEMAFilter.AddValue(current);
      current1 = singleEMAFilter.GetLowPass();

      if (digitalRead(BTN1) == 0 and digitalRead(BTN2) == 0) { //run
        if (vbat > 9.0 and current1 < 180) { //180 -25
          analogWrite(MOTOR, 255);
          delay(80);
          //}
        } else {
          analogWrite(MOTOR, 0);
          digitalWrite(ORANGE, HIGH);
          digitalWrite(GREEN, HIGH);
          delay(100);
          digitalWrite(ORANGE, LOW);
          digitalWrite(GREEN, LOW);
          delay(100);
          digitalWrite(ORANGE, HIGH);
          digitalWrite(GREEN, HIGH);
          delay(100);
          digitalWrite(ORANGE, LOW);
          digitalWrite(GREEN, LOW);
          delay(100);
          digitalWrite(ORANGE, HIGH);
          digitalWrite(GREEN, HIGH);
          delay(100);
          digitalWrite(ORANGE, LOW);
          digitalWrite(GREEN, LOW);
          delay(200);
          resetFunc();
        }

      } else if (digitalRead(BTN1) == 0 ) {
        analogWrite(MOTOR, 0);
        mVolt();
      } else {
        analogWrite(MOTOR, 0);
        digitalWrite(GREEN, LOW);
        //Serial.println(digitalRead(A2));

        //if (digitalRead(A2) == 1) { //if charging
        //  mVolt();
        //}

        unsigned long period = 5000;

        static unsigned long previousMillis = 0;

        if ((millis() - previousMillis) > period) {    //deep sleep
          //flagStart = true; // run softart

          // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
          //static byte prevADCSRA = ADCSRA;
          //ADCSRA = 0;
          ADCSRA &= ~(1 << ADEN);

          set_sleep_mode (SLEEP_MODE_PWR_DOWN);
          sleep_enable();

          MCUCR = bit (BODS) | bit (BODSE);

          // The BODS bit is automatically cleared after three clock cycles so we better get on with it
          MCUCR = bit (BODS);

          cli();
          PcInt::attachInterrupt(BTN2, wakeUpNow, CHANGE);//attaching a interrupt to pin d6
          PcInt::attachInterrupt(BTN1, mVolt, CHANGE);//attaching a interrupt to pin d7
          PcInt::attachInterrupt(VUSB, measureVolClick, CHANGE);//attaching a interrupt to pin d7

          // Send a message just to show we are about to sleep
          Serial.println("Deep sleep");
          Serial.flush();

          // Allow interrupts now
          sei();

          // And enter sleep mode as set above
          sleep_cpu();

          // --------------------------------------------------------
          // Microcontroller is now a sleep until woken up by an interrupt
          // --------------------------------------------------------

          // Wakes up at this point when wakePin is brought LOW - interrupt routine is run first
          Serial.println("I'm awake!");

          // Re-enable ADC if it was previously running
          ADCSRA |= (1 << ADEN);
          previousMillis += period;
        }
      }



      break;
    case CHARGE:
      //Serial.println(analogRead(VUSB) );

      vbat = (float)analogRead(A0) / 1023 * 12.0 * (30921.053 / 10000);
      if (analogRead(VUSB) >= 800) {
        if (vbat > 10) {
          digitalWrite(GREEN, HIGH);
          digitalWrite(ORANGE, LOW);
        } else {
          digitalWrite(GREEN, LOW);
          digitalWrite(ORANGE, HIGH);
        }
      } else {
        digitalWrite(GREEN, LOW);
        digitalWrite(ORANGE, LOW);
        stateOp = NORMAL;
        resetFunc();
      }

      break;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////
  if (flagMvolt) {

    currentMillis6 = millis();
    if (currentMillis6 - previousMillis6 < 200) {
      // LED GREEN OFF
      digitalWrite(GREEN, LOW);
      digitalWrite(ORANGE, LOW);
      flagMvolt = false;
    } else if (currentMillis6 - previousMillis6 < 1000) {
      // LED GREEN ON
      vbat = (float)analogRead(A0) / 1023 * 12.0 * (30921.053 / 10000);

      if (vbat > 10) {
        digitalWrite(GREEN, HIGH);
        digitalWrite(ORANGE, LOW);
      } else {
        digitalWrite(GREEN, LOW);
        digitalWrite(ORANGE, HIGH);
      }

    } else {
      previousMillis6 = currentMillis6;
    }

  }


}

void mVolt() {
  static unsigned long lastintetime = 0;
  unsigned long interruptiontime = millis();
  if (interruptiontime - lastintetime > 150)
  {
    flagMvolt = true;
  }
  lastintetime = interruptiontime;

}

void measureVolClick() {
  sleep_disable();

  // Re-enable ADC if it was previously running
  ADCSRA |= (1 << ADEN);
  Serial.println(analogRead(VUSB) );
  Serial.println("PCINT----------------------------------------------------");
  if (analogRead(VUSB) >= 300) {
    stateOp = CHARGE;
  }

  PcInt::detachInterrupt(BTN1);
  PcInt::detachInterrupt(BTN2);
  PcInt::detachInterrupt(VUSB);
}

// When wakePin is brought LOW this interrupt is triggered FIRST (even in PWR_DOWN sleep)
void wakeUpNow() {
  //flagStart = true; // run softart
  // Prevent sleep mode, so we don't enter it again, except deliberately, by code
  sleep_disable();
  ADCSRA |= (1 << ADEN);
  // Detach the interrupt that brought us out of sleep
  PcInt::detachInterrupt(BTN1);
  PcInt::detachInterrupt(BTN2);
  PcInt::detachInterrupt(VUSB);
  // Now we continue running the main Loop() just after we went to sleep
}
