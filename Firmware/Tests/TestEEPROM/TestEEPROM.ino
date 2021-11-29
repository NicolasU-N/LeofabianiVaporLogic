//https://forum.arduino.cc/t/how-to-store-decimal-value-in-eeprom/392036/11

#include <EEPROM.h>

void setup()
{
  Serial.begin(115200);

  int f = 1;
  int i = 350;  
  int j = 255;


  int x;
  int y;
  int z;

  EEPROM.put(0, 1); // °C or °F
  EEPROM.put(1, i); // Setpoint
  EEPROM.put(1 + sizeof(float), j); // brightness

  
  Serial.print("Read Address 0: ");
  Serial.println(EEPROM.read(0));

  EEPROM.get(1, y);
  Serial.print("Read Address 1: ");
  Serial.println(y);

  EEPROM.get(1 + sizeof(int), z);
  Serial.print("Read Address 2: ");
  Serial.println(z);

}

void loop()
{

}
