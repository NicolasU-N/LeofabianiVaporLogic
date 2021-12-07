#define UTEMP       A2

float utemp = 0;

void setup() {
  Serial.begin(115200);
  pinMode(UTEMP, INPUT);
}

void loop() {
  float readUTemp = (analogRead(UTEMP) / 1023.0) * 5.0 * 1000; //convert mV
  utemp = -0.000010541 * pow(readUTemp, 2) -0.1636 * (readUTemp) + 180.3525;
  Serial.print("readUTemp = ");
  Serial.println(readUTemp);

  Serial.print("utemp = ");
  Serial.println(utemp);

  delay(300);
}
