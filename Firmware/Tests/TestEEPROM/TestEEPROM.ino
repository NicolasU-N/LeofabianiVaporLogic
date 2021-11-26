//https://forum.arduino.cc/t/how-to-store-decimal-value-in-eeprom/392036/11

#include <EEPROM.h>

void setup()
{
  Serial.begin(9600);

  int i = 1024;
  float f = 3.14;


  int j;
  float g;

  EEPROM.put(0, i);
  EEPROM.put(0 + sizeof(int), f);

  EEPROM.get(0, j);
  Serial.println(j, DEC);

  EEPROM.get(0 + sizeof(int), g);
  Serial.println(g, 3);

}

void loop()
{

}
