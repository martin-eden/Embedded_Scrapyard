// Approximating rpm by counting peaks via ISR

/*
  Status: ok
  Last mod.: 2017-06-22
*/

/*
  Based on horribly code by Crenn from http://thebestcasescenario.com

  http://www.themakersworkbench.com/content/tutorial/reading-pc-fan-rpm-arduino

  2017-06-22
*/

const int measure_delay_ms = 12000;
const uint8_t triggers_per_rotation = 2;

const uint8_t sensor_pin = 2; // hard-wired for Uno, other values won't work
const uint8_t interrupt_number = 0; // depends on <sensor_pin>

const float measures_per_second = 1000 / (float)measure_delay_ms;
const float measures_per_minute = 60 * measures_per_second;

volatile uint16_t rpm_counter;

void pin_handler()
{
  rpm_counter++;
}

void setup()
{
  pinMode(sensor_pin, INPUT);
  attachInterrupt(interrupt_number, pin_handler, RISING);
  Serial.begin(9600);
}

void loop()
{
  rpm_counter = 0;
  delay(measure_delay_ms);
  float rpm_approx = rpm_counter * measures_per_minute / triggers_per_rotation;

  Serial.print(rpm_approx);
  Serial.print(" rpm");

  Serial.print(" (");
  Serial.print(rpm_counter);
  Serial.print(")");
  Serial.print("\n");
}
