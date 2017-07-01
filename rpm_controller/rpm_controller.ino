// Controlling rpm by potentiometer

/*
  Status: works
  Last mod.: 2017-06-23
*/

const uint8_t sensor_pin = 2; // hard-wired for Uno, other values won't work
const uint8_t potentiometer_pin = A0;
const uint8_t speed_pin = 3; // must support PWM output
const int measure_delay_ms = 3750;

const float max_trustable_rpm = 8000; // set to near 3x of real max rpm (1)
const uint8_t triggers_per_rotation = 2;

const uint8_t interrupt_number = 0; // depends on <sensor_pin>
const float measures_per_second = 1000 / (float)measure_delay_ms;
const float measures_per_minute = 60 * measures_per_second;

const float max_measures_per_interval = max_trustable_rpm * triggers_per_rotation / measures_per_minute;
const uint32_t min_trustable_measure_gap_mcrs = measure_delay_ms / max_measures_per_interval * 1000;

volatile uint16_t rpm_counter;
volatile uint32_t last_micros;

void pin_handler()
{
  uint32_t cur_micros = micros();
  if (cur_micros - last_micros > min_trustable_measure_gap_mcrs)
  {
    rpm_counter++;
  }
  last_micros = cur_micros;
}

void setup()
{
  pinMode(sensor_pin, INPUT);
  attachInterrupt(interrupt_number, pin_handler, RISING);
  pinMode(potentiometer_pin, INPUT);
  pinMode(speed_pin, OUTPUT);

  Serial.begin(9600);
  Serial.print("min_trustable_measure_gap_mcrs: ");
  Serial.print(min_trustable_measure_gap_mcrs);
  Serial.print("\n");
}

void loop()
{
  rpm_counter = 0;
  last_micros = micros();
  delay(measure_delay_ms);
  float rpm_approx = rpm_counter * measures_per_minute / triggers_per_rotation;

  uint16_t pot_val = analogRead(potentiometer_pin);
  uint16_t speed_val = pot_val >> 2;
  uint8_t pot_perc = ((float)100 * (pot_val + 1) + 5) / 1024; // "+ 5" ~= .5%

  analogWrite(speed_pin, speed_val);

  Serial.print("\n");
  Serial.print("potentiometer_%: ");
  Serial.print(pot_perc);

  Serial.print("; ");
  Serial.print("num_measurements: ");
  Serial.print(rpm_counter);

  Serial.print("; ");
  Serial.print("rpm: ");
  Serial.print(rpm_approx);
}

/*
  :[1]:

  Somewhy setting <max_trustable_rpm> near really achievable rpm
  yields to speed tracking blackout near middle PWM duty cycle
  (i.e. when potentiometer knob between say 20% and 70% tracked
  speed is 0 rpm due "if" in pin_handler()).

    I don't know why it is so.

  Raising <max_trustable_rpm> reduces trust gap so more measurements
  are tracked.
*/
