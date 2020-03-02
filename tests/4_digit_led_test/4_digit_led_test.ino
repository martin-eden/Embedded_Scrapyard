// Test 4-digit LED by printing value from analog pin

/*
  Status: stable
  Last mod.: 2020-03-02
*/

#include <TM1637Display.h>

const uint8_t
  INPUT_PIN = A4,
  CLOCK_PIN = A5,

  ANALOG_PIN = A0;

const uint16_t
  MEASUREMENT_DELAY_MS = 217;

TM1637Display display(CLOCK_PIN, INPUT_PIN);

void setup() {
  display.setBrightness(0x0F);
}

void loop() {
  uint16_t value = analogRead(ANALOG_PIN);
  display.showNumberDec(value, false);
  delay(MEASUREMENT_DELAY_MS);
}
