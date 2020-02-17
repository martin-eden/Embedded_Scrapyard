#include <TM1637Display.h>

const uint8_t
  input_pin = A4,
  clock_pin = A5,

  analog_pin = A0;

TM1637Display display(clock_pin, input_pin);

void setup()
{
  display.setBrightness(0x0F);
}

void loop()
{
  uint16_t value = analogRead(analog_pin);
  display.showNumberDec(value, true);
}
