/** @file     LTC4624.h
    @brief    Dynamic lighting for the 3-digit 7-segment LED display
    @Author   Nicolas
    Created:  23/11/2021 12:32:10
    Forked: https://github.com/victornpb/multiplexed-7segments-display/blob/master/sevenSeg.ino
*/

#ifndef LTC4624_h
#define LTC4624_h

#include <Wire.h>
#include "PCF8574.h"  // https://github.com/xreef/PCF8574_library
#include "PWM.h"

#define ADDR_PCF        0x20

//Change settings to match Common Cathode/Anode
#define SEGMENT_ON HIGH
#define COMMON_ON LOW

#define ALPHA 0 //change to 1 to include leters 

class LTC4624 {

  public:
    uint8_t dutyCycleLcd;
    LTC4624();
    void lcdScan();
    void lcdChar(uint8_t pos, uint8_t sign);
    void lcdPrint(uint8_t pos, const char *str);
    void lcdDot(uint8_t pos, uint8_t dot);
    void lcdOff();
    void lcdClear();

};
#endif
