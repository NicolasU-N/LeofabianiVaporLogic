#include <Wire.h>
#include "PCF8574.h"

//Controllers 7-seg
#define DISPLAY_A1 11
#define DISPLAY_A2 9
#define DISPLAY_A3 10

byte  numero[10][8] =
{
  { 1, 1, 1, 1, 1, 1, 0, 0 }, // 0
  { 0, 1, 1, 0, 0, 0, 0, 0 }, // 1
  { 1, 1, 0, 1, 1, 0, 1, 0 }, // 2
  { 1, 1, 1, 1, 0, 0, 1, 0 }, // 3
  { 0, 1, 1, 0, 0, 1, 1, 0 }, // 4
  { 1, 0, 1, 1, 0, 1, 1, 0 }, // 5
  { 1, 0, 1, 1, 1, 1, 1, 0 }, // 6
  { 1, 1, 1, 0, 0, 0, 0, 0 }, // 7
  { 1, 1, 1, 1, 1, 1, 1, 0 }, // 8
  { 1, 1, 1, 0, 0, 1, 1, 0 } // 9
};

PCF8574 PCF(0x20);

void setup() {
  Serial.begin(9600);
  PCF.begin();
  delay(100);

  

}

void loop() {
  // put your main code here, to run repeatedly:
  using_pcf_numeric(423);
}

void generar_simbolo(int caracter){
  Serial.println(caracter);
  for (uint8_t segmento = 0; segmento <= 7; segmento++){
    /*
    Serial.print("Encendiendo segmento: ");
    Serial.print(segmento);
    Serial.print(" con el valor de: ");
    Serial.print(!(bool)numero[caracter][segmento]);
    Serial.print("\n");*/
    PCF.digitalWrite(segmento,!(bool)numero[caracter][segmento]);
  }
}

/*
void dividir_numero(int Valor){
  int Centena, Decena, Unidad;
  Centena = Valor/100;
  Decena = (Valor - Centena*100)/10;
  Unidad = (Valor - Centena*100 - Decena*10);
  Serial.print("Unidad: ");
  Serial.println(Unidad);
  Serial.print("Decena: ");
  Serial.println(Decena);
  Serial.print("Centena: ");
  Serial.println(Centena);
  }
*/

void controlling_display(uint8_t A1_value,uint8_t A2_value,uint8_t A3_value){
  //Centena Decena y Unidad
  analogWrite(DISPLAY_A1, A1_value);
  analogWrite(DISPLAY_A2, A2_value); 
  analogWrite(DISPLAY_A3, A3_value); 
}

void using_pcf_numeric(int Valor){
  int delay_mux = 15;
  int Centena, Decena, Unidad;
  Centena = Valor/100;
  Decena = (Valor - Centena*100)/10;
  Unidad = (Valor - Centena*100 - Decena*10);
  
  controlling_display(0,0,255);
  generar_simbolo(Unidad);
  delay(delay_mux);

  controlling_display(0,255,0);
  generar_simbolo(Decena);
  delay(delay_mux);
  

  controlling_display(255,0,0);
  generar_simbolo(Centena);
  delay(delay_mux);
  
}
