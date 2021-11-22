#define LMT84_PIN A0


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  lmt84_temperature();
}

int lmt84_temperature(){
  int temperature = analogRead(LMT84_PIN);
  Serial.println(temperature);
  delay(1);  
}
