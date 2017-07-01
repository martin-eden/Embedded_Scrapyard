#include <Arduino.h>
#include "humidity_measurer.h"

const uint32_t line_problem_threshold = 10;

humidity_measurer::humidity_measurer() {
  sensor_pin = A0;
  power_pin = 2;
  min_value = 0;
  max_value = 680;
  power_off_between_measures = false;
  high_means_dry = false;
  is_line_problem = false;
}

int humidity_measurer::get_raw_value() {
  int raw_value;
  if (power_off_between_measures) {
    digitalWrite(power_pin, HIGH);
    delay(50);
    raw_value = analogRead(sensor_pin);
    digitalWrite(power_pin, LOW);
  }
  else {
    raw_value = analogRead(sensor_pin);
  }

  is_line_problem = (raw_value <= line_problem_threshold);

  return raw_value;
}

const int aligned_low = 0;
const int aligned_high = 100;

int humidity_measurer::get_value() {
  int result;
  result = get_raw_value();
  result = constrain(result, min_value, max_value);
  if (high_means_dry)
    result = map(result, min_value, max_value, aligned_high, aligned_low);
  else
    result = map(result, min_value, max_value, aligned_low, aligned_high);
  return result;
}
