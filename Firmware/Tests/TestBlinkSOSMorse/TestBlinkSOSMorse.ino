unsigned long t = 0;
unsigned long t_blink = 0;

#define LEDRED      7

void setup() {
  pinMode(LEDRED, OUTPUT);
}

void loop() {

  if (t - t_blink < 200)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 400)
    digitalWrite(LEDRED, LOW);  // .
  else if (t - t_blink < 600)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 800)
    digitalWrite(LEDRED, LOW);  // .
  else if (t - t_blink < 1000)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 1200)
    digitalWrite(LEDRED, LOW);  // .
  else if (t - t_blink < 1700)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 2200)
    digitalWrite(LEDRED, LOW);  // -
  else if (t - t_blink < 2700)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 3200)
    digitalWrite(LEDRED, LOW); // -
  else if (t - t_blink < 3700)
    digitalWrite(LEDRED, HIGH);
  else if (t - t_blink < 4200)
    digitalWrite(LEDRED, LOW); // -
  else
    t_blink = t;
}
