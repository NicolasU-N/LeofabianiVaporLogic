#include "LTC4624.h"

// Set i2c address
PCF8574 pcf8574(ADDR_PCF); //&Wire,

byte displayData[3]; //hold current segments information
byte currentSegment; //number of the current segment in the multiplexing
byte currentDigit; //multiplexing for multiple digits
long previousMicros = 0; //variable to keep track of interval between segments

const uint8_t displayCount = 3;

const uint8_t displayPins[8] = {
  P0, //A
  P1, //B
  P2, //C
  P3, //D
  P4, //E
  P5, //F
  P6, //G
  P7  //H (Point)
};


//LTC4624::LTC4624() {
//}


/**
  set pinsMode
*/

void LTC4624::sevenSegSetup() {

  uint8_t i = 8;
  while (i--)
    pcf8574.pinMode(displayPins[i], OUTPUT, 1);

  if (!pcf8574.begin()) {
    Serial.println("ERROR PCF8574");
  }
}


/*

*/
void LTC4624::setNumber(unsigned int n) {
  setDigit(2, sevenSeg_font[n % 10]);
  setDigit(1, sevenSeg_font[n / 10 % 10]);
  setDigit(0, sevenSeg_font[n / 100 % 10]);
}

/** Set a display digit
  @param uint8_t digit - Display Index
  @param byte character - 7bit 0 paded character
*/
void LTC4624::setDigit(uint8_t digit, byte character) {
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

    sevenSegdisplay();
  }
}

/** function that perform the multiplexing to draw every digit */
void LTC4624::sevenSegdisplay() {
  currentSegment = scanSegments(displayData[currentDigit], currentSegment);
  Serial.print("ON ");
  Serial.println(currentSegment);

  //change displays after scaning every segment on a single digit
  if (currentSegment > 7) {
    Serial.print("CHANGE");
    Serial.println(currentSegment);
    currentSegment = -1;
    switchDisplay();
  }
}

/** switch to next display */
void LTC4624::switchDisplay() {

  //switch off current display

  //pinMode(displayCommonPins[currentDigit], INPUT);

  setPWMdisplay(0);
  
  currentDigit = (currentDigit + 1) % displayCount; //next
  
  setPWMdisplay(dutyCycleLcd);


  //pinMode(displayCommonPins[currentDigit], OUTPUT);
  //digitalWrite(displayCommonPins[currentDigit], COMMON_ON);
}


/** function that advance to each segment every time this is called */
int LTC4624::scanSegments(byte data, uint8_t segmentNum) {

  byte segment;
  bool state;

  pcf8574.digitalWrite(displayPins[segmentNum], !SEGMENT_ON); //turn previous segment OFF

  do {
    ++segmentNum; //cicle through the segments 0 to 7
    if (segmentNum > 8) return segmentNum; //end, go to next digit

    segment = data & (1 << (7 - segmentNum)); //apply a mask to select just the desired segment bit
    state = (segment >> (7 - segmentNum)); //shift the bit to LSF to get the boolean value
  }
  while (!state); //skip the dead segments time

  pcf8574.digitalWrite(displayPins[segmentNum], SEGMENT_ON); //turn segment ON

  return segmentNum;
}

void setPWMdisplay(uint8_t dutylcd) {
  switch (currentDigit) {
    case 0:
      setDutyPWMPB3(dutylcd);
      break;
    case 1:
      setDutyPWMPB1(dutylcd);
      break;
    case 2:
      setDutyPWMPB2(dutylcd);
      break;
  }
}
//-------------------------------------------
void LTC4624::setDutyCycleLcd(uint8_t val)
{
  this->dutyCycleLcd = val;
}

uint8_t LTC4624::getDutyCycleLcd()
{
  return dutyCycleLcd;
}

byte LTC4624::getSevenSegFont(uint8_t id) {
  return sevenSeg_font[id];
}
