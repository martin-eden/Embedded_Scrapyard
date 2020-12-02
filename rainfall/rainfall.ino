// Rainfall imitation

/*
  Status: testing
  Version: 0.4
  Last mod.: 2020-12-02
*/

/*
  Intend to pour water every day (at receiving alarm signal from DS3231.
  Be sure it triggers once a day!).

  Watering duration is calculated from
    (1) water pump volume, ml per minute
    (2) pot diameter, mm
    (3) desired water precipation per day, mm
  These values are in const section.

  Hardware setup:
    * DS3231 - configure alarm output to pin, connect with <TIMER_PIN>
    * Switch relay - connect control to <MOTOR_PIN>
*/

/*
  Third project in my gardfening series.
  First - flower friend - many expreimental features, cluttered code.
  Second - pour manager - soil hydrostat, most features ripped.
  Third - rainfall - pour predefined amount at timer event, duration calculation.
*/

#include <me_ds3231.h>
#include <me_switch.h>
#include <me_PourDurationCalculator.h>

const uint8_t
  TIMER_PIN = 2,
  MOTOR_PIN = 4;

const float
  pumpingVolume_mlPerMinute = 100,
  potDiameter_mm = 205,
  precipation_mm = (float) 52 / 30;
/*
  monthPrecipation, int, mm
    36 40 54 58 59 62 47 64 62 59 49 34
    avg 52
    # https://en.wikipedia.org/wiki/Port_Elizabeth
*/

me_ds3231 rtc;
c_switch motor = c_switch(MOTOR_PIN);
me_PourDurationCalculator PourDurationCalculator;

volatile bool isTimeToWork;

void armTrap() {
  rtc.clearAlarm1Line();
  rtc.clearAlarm2Line();
  isTimeToWork = false;
  Serial.println("Waiting for timer signal.");
}

void setup() {
  Serial.begin(9600);
  pinMode(TIMER_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(TIMER_PIN), rain_time_event, FALLING);
  armTrap();
  Serial.println("Setup done.");
}

void loop() {
  if (isTimeToWork) {
    doWork();
    armTrap();
  }
}

void doWork() {
  int32_t pouringDuration;
  pouringDuration =
    PourDurationCalculator.calculatePouringDuration_ms(
      pumpingVolume_mlPerMinute,
      potDiameter_mm,
      precipation_mm
    );

  Serial.print("Working for ");
  Serial.print(pouringDuration);
  Serial.println(" ms.");
  motor.switch_on();
  delay(pouringDuration);
  motor.switch_off();
  Serial.println("Work done.");
}

void rain_time_event() {
  isTimeToWork = true;
}
