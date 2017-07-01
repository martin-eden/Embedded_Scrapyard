// <max7219> module and device tester

/*
  Status: done
  Last mod.: 2015-10-13
*/

#include <Arduino.h>
#include "max7219.h"
//#include "xspi.h"

const byte num_modules = 11;

const byte out_pin = 2;
const byte cs_pin = 3;
const byte clk_pin = 4;

max7219_modules_driver led_driver;
spi_transciever spi_channel;
max7219_operation command_buffer[num_modules];

void setup()
{
  // Serial.begin(9600);
  // Serial.print("Init done.\n");

  spi_channel.out_pin = out_pin;
  spi_channel.cs_pin = cs_pin;
  spi_channel.clk_pin = clk_pin;
  spi_channel.init_pins();
  led_driver.spi_channel = &spi_channel;
  led_driver.command_buffer = &command_buffer[0];
  led_driver.set_num_modules(num_modules);
  for (byte i = 0; i <= num_modules - 1; i++)
  {
    led_driver.set_scan_limit(i, 7);
    led_driver.set_intensity(i, 0);
    led_driver.display_test_off(i);
    for (byte j = 0; j <= 7; j++)
      led_driver.set_row(i, j, 0);
    led_driver.show(i);
  }
  led_driver.apply();
}

void loop()
{
  for (byte i = 0; i <= num_modules - 1; i++)
  {
    led_driver.display_test_on(i);
    for (byte j = 0; j <= 7; j++)
      led_driver.set_row(i, j, (1 << j));
      // led_driver.set_row(i, j, (i + j) | (1 << j));
    led_driver.apply();
    delay(1500);
    led_driver.display_test_off(i);
    led_driver.apply();
  }
}
