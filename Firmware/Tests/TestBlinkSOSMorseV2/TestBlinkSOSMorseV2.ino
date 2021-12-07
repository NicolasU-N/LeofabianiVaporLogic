#include <TaskScheduler.h> //https://github.com/arkhipenko/TaskScheduler

Scheduler runner;

void t0Callback();

Task t0(200, 18, &t0Callback, &runner, true);


#define LEDRED      7

//---------------------------------------------------------------------------------------- TASKS
void t0Callback() {  
  t0.getRunCounter() > 5 and t0.getRunCounter() < 13 ? t0.setInterval(500) : t0.setInterval(200);

  if ( t0.getRunCounter() & 1 ) {
    digitalWrite(LEDRED, LOW);
  }
  else {
    digitalWrite(LEDRED, HIGH);
  }

  if ( t0.isLastIteration() ) {
    t0.restartDelayed( 1 * TASK_SECOND );
    digitalWrite(LEDRED, HIGH);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LEDRED, OUTPUT);
  runner.startNow();
}

void loop() {
  runner.execute();
}
