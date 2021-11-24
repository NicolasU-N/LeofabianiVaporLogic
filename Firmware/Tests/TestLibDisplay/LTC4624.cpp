#include "LTC4624.h"

// Set i2c address
PCF8574 pcf8574(ADDR_PCF); //&Wire,

byte displayData[3]; //hold current segments information
byte currentSegment; //number of the current segment in the multiplexing
byte currentDigit; //multiplexing for multiple digits
long previousMicros = 0; //variable to keep track of interval between segments

const byte sevenSeg_font[] = {
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9

  B00000010, // dash -
  //B00010000, // underscore _

#if ALPHA==1
  B11101110, //A
  B00111110, //b
  B10011100, //C
  B00011010, //c
  B01111010, //d
  B10011110, //E
  B10001110, //F
  B01101110, //H
  B00101110, //h
  B00011100, //L
  B01100000, //l
  B11111100, //O
  B00111010, //o
  B11001110, //P
  B10110110, //S
#endif

  B00000000,  // blank
};

/**
  set pinsMode
*/

LTC4624::LTC4624() {

  uint8_t i = 8;
  while (i--)
    pcf8574.pinMode(displayPins[i], OUTPUT);

  /*
    pcf8574.pinMode(SEG_A_PIN, OUTPUT);
    pcf8574.pinMode(SEG_B_PIN, OUTPUT);
    pcf8574.pinMode(SEG_C_PIN, OUTPUT);
    pcf8574.pinMode(SEG_D_PIN, OUTPUT);
    pcf8574.pinMode(SEG_E_PIN, OUTPUT);
    pcf8574.pinMode(SEG_F_PIN, OUTPUT);
    pcf8574.pinMode(SEG_G_PIN, OUTPUT);
    pcf8574.pinMode(SEG_H_PIN, OUTPUT);
  */

  Wire.begin();

  if (pcf8574.begin()) {
    Serial.println("ERROR PCF8574");
  }
}

/*

*/
void LTC4624::setNumber(unsigned int n) {
  sevenSeg_setDigit(1, sevenSeg_font[n / 10]);
  sevenSeg_setDigit(0, sevenSeg_font[n % 10]);
}

/** Set a display digit
  @param uint8_t digit - Display Index
  @param byte character - 7bit 0 paded character
*/
void sevenSeg_setDigit(uint8_t digit, byte character) {
  displayData[digit] = character | (displayData[digit] & B00000001); //set digit without changing the decimal point bit
}


/** Toggle the decimal point on/off
  @param uint8_t digit - Display Index
  @param bool on - state true/false
*/
void LTC4624::setDecimalPoint(uint8_t digit, bool on) {
  displayData[digit] = (displayData[digit] | 1) & (on ? B11111111 : B11111110); //flip the decimalPoint bit without changing the rest
}


/**
  Function that perform the multiplexing to draw every digit with a assyncronous delay between segments
  if your program have enough delay, call sevenSeg_display() directly instead.
  @param unsigned int us - adjust the interval between segments in microseconds
*/
void LTC4624::displayHold(unsigned int us) {

  unsigned long currentMicros = micros();
  //unsigned long currentMillis = millis();

  if (currentMicros - previousMicros > us) {
    previousMicros = currentMicros;

    sevenSeg_display();
  }
}

/** function that perform the multiplexing to draw every digit */
void sevenSeg_display() {
  currentSegment = sevenSeg_scanSegments(displayData[currentDigit], currentSegment);

  //change displays after scaning every segment on a single digit
  if (currentSegment > 7) {
    currentSegment = -1;
    sevenSeg_switchDisplay();
  }
}

/** switch to next display */
void sevenSeg_switchDisplay() {

  //switch off current display
  pinMode(displayCommonPins[currentDigit], INPUT);

  currentDigit = (currentDigit + 1) % displayCount; //next

  //pinMode(displayCommonPins[currentDigit], OUTPUT);
  digitalWrite(displayCommonPins[currentDigit], COMMON_ON);
}


/** function that advance to each segment every time this is called */
int sevenSeg_scanSegments(byte data, uint8_t segmentNum) {

  byte segment;
  bool state;

  digitalWrite(displayPins[segmentNum], !SEGMENT_ON); //turn previous segment OFF

  do {
    ++segmentNum; //cicle through the segments 0 to 7
    if (segmentNum > 8) return segmentNum; //end, go to next digit

    segment = data & (1 << (7 - segmentNum)); //apply a mask to select just the desired segment bit
    state = (segment >> (7 - segmentNum)); //shift the bit to LSF to get the boolean value
  }
  while (!state); //skip the dead segments time

  digitalWrite(displayPins[segmentNum], SEGMENT_ON); //turn segment ON

  return segmentNum;
}
