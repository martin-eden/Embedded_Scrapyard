#include <me_ds3231.h>
#include <time.h>
#include <util/usa_dst.h>

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

me_ds3231 RTC;

void setup()
{
  Serial.begin(115200);
  Serial.println("System library time routines test.");

  SetTime();

  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, RISING);
}

void OnSignalChange()
{
  system_tick();
}

void SetTime()
{
  DateTime dt;
  dt = RTC.getDateTime();
  uint32_t unix_time = dt.unixtime(); // + (uint32_t) 1400 * ONE_HOUR;
  set_system_time(unix_time - UNIX_OFFSET);
  set_zone(-5 * ONE_HOUR);
  set_dst(usa_dst);
  set_position(43.58 * ONE_DEGREE, -79.65 * ONE_DEGREE);
}

void DoBusiness()
{
  time_t Timer = time(NULL);
  time_t Timer2;

  Serial.print("time\t");
  Serial.println(Timer);

  Serial.print("isotime\t");
  Serial.println(isotime(gmtime(&Timer)));

  Serial.print("equation_of_time\t");
  Serial.println(equation_of_time(&Timer));

  Serial.print("daylight_seconds\t");
  Serial.println(daylight_seconds(&Timer));

  Timer2 = sun_rise(&Timer) - 4 * ONE_HOUR;
  Serial.print("sun_rise\t");
  Serial.println(isotime(gmtime(&Timer2)));

  Timer2 = solar_noon(&Timer) - 4 * ONE_HOUR;
  Serial.print("solar_noon\t");
  Serial.println(isotime(gmtime(&Timer2)));

  Timer2 = sun_set(&Timer) - 4 * ONE_HOUR;
  Serial.print("sun_set\t");
  Serial.println(isotime(gmtime(&Timer2)));

  Serial.print("solar_declination\t");
  Serial.println(solar_declination(&Timer));

  Serial.print("moon_phase\t");
  Serial.println(moon_phase(&Timer));

  Serial.print("gm_sidereal\t");
  Serial.println(gm_sidereal(&Timer));

  Serial.print("lm_sidereal\t");
  Serial.println(lm_sidereal(&Timer));

}

void loop()
{
  DoBusiness();
  delay(30000);
}
