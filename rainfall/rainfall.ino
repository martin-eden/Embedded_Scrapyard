// Rainfall imitation

/*
  Status: testing
  Version: 0.3
  Last mod.: 2020-11-20
*/

/*
  Third project in my gardfening series.
  First - flower friend - many expreimental features, cluttered code.
  Second - pour manager - soil hydrostat, most features ripped.
  Third - rainfall - pour predefined amount at timer event.
*/

/*
  Hardware setup:
    * DS3231 - configure alarm output to pin, connect with <TIMER_PIN>
    * Switch relay - connect control to <MOTOR_PIN>
*/

#include <me_ds3231.h>
#include <me_switch.h>

const uint8_t
  TIMER_PIN = 2,
  MOTOR_PIN = 4;

me_ds3231 rtc;
c_switch motor = c_switch(MOTOR_PIN);

volatile bool isTimeToWork;

void reloadTrap() {
  isTimeToWork = false;
  rtc.clearAlarm1Line();
  rtc.clearAlarm2Line();
  Serial.println("Waiting for timer signal.");
}

void setup() {
  Serial.begin(9600);
  pinMode(TIMER_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(TIMER_PIN), rain_time_event, FALLING);
  reloadTrap();
  Serial.println("Setup done.");
}

void loop() {
  if (isTimeToWork) {
    doWork();
    reloadTrap();
  }
}

void doWork() {
  Serial.println("Working.");
  motor.switch_on();
  delay(20000);
  motor.switch_off();
  Serial.println("Work done.");
}

void rain_time_event() {
  isTimeToWork = true;
}
