/** @file     LTC4624.h
    @brief    Dynamic lighting for the 3-digit 7-segment LED display
    @Author   Nicolas
    Created:  23/11/2021 12:32:10
    Forked: https://github.com/victornpb/multiplexed-7segments-display/blob/master/sevenSeg.ino
*/

#ifndef LTC4624_h
#define LTC4624_h

#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library
#include "PWM.h"

#define ADDR_PCF        0x20

//Change settings to match Common Cathode/Anode
#define SEGMENT_ON LOW //HIGH

void setPWMdisplay(uint8_t dutylcd);

class LTC4624 {

  private:
    uint8_t dutyCycleLcd;
    void switchDisplay();
    int scanSegments(byte data, uint8_t segmentNum);

    byte sevenSeg_font[27] = {
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
      B00000000,  // blank
    };

  public:
    //LTC4624();

    void sevenSegSetup();
    void setDutyCycleLcd(uint8_t val);
    uint8_t getDutyCycleLcd();
    byte getSevenSegFont(uint8_t id);

    void setNumber(unsigned int n);
    void setDigit(uint8_t digit, byte character);

    void setDecimalPoint(uint8_t digit, bool on);
    void displayHold(unsigned int us);
    void sevenSegdisplay();

};
#endif
