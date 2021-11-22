#define sw1 A1
#define sw2 A3

int buttonState = 0;

/*
0 for no one pressed
1 for sw1 pressed
2 for sw2 pressed
3 for both pressed
*/

int sw1_state = 1;
int sw2_state = 1;

int lastButtonState = 0; // previous state of the button

int startPressed = 0;    // the moment the button was pressed
int endPressed = 0;      // the moment the button was released
int holdTime = 0;        // how long the button was hold
int idleTime = 0;        // how long the button was idle

//Debouncing button state:
unsigned long previousMillis = 0;  
const long interval = 1000; 

void setup() {
  pinMode(sw1, INPUT); // initialize the button pin as a input
  pinMode(sw2, INPUT); // initialize the button pin as a input
  Serial.begin(9600);        // initialize serial communication
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sw_status();
  }
  delay(50);
}

void updateState() {
  // the button has been just pressed

  if (buttonState != 0) {
      Serial.print("Button stated has changed to: ");
      Serial.println(buttonState);
      startPressed = millis();
      idleTime = startPressed - endPressed;

  // the button has been just released
  } else {
      endPressed = millis();
      holdTime = endPressed - startPressed;

      //if (holdTime >= 100 && holdTime < 1000) {
      if (holdTime < 2000) {
          Serial.print("Button "); Serial.print(lastButtonState);
          Serial.println(" was held for less than a second"); 
      }

      if (holdTime >= 3000) {
          Serial.print("Button "); Serial.print(lastButtonState);
          Serial.println(" was held for three second or more"); 
      }

  }
}


void sw_status(){
  //Reading Buttons
  sw1_state = digitalRead(sw1);
  sw2_state = digitalRead(sw2);
  
  if(sw1_state && sw2_state){
    //Both in high = no one is pressed
    buttonState = 0;
    }
   else if(!sw1_state && sw2_state){
    buttonState = 1;
    }
   else if(sw1_state && !sw2_state){
    buttonState = 2;
    }
   else{
    buttonState = 3;
    }

  if (buttonState != lastButtonState) { // button state changed
     updateState();
  }

  lastButtonState = buttonState;        // save state for next loop

}
