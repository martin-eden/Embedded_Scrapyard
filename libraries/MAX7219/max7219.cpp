// Driver for MAX7219 8x8-segment LEDs microprocessor

/*
  Status: stable
  Last mod.: 2015-10-14
*/

#include <Arduino.h>
#include "max7219.h"
#include "xspi.h"

#define CMD_NOOP 0
#define CMD_SET_BCD 9
#define CMD_SET_INTENSITY 10
#define CMD_SET_SCAN_LIMIT 11
#define CMD_SHOW 12
#define CMD_DISPLAY_TEST 15


void max7219_modules_driver::init_command_buffer()
{
  for (unsigned int i = 0; i < num_modules; i++)
  {
    command_buffer[i].command = CMD_NOOP;
    command_buffer[i].data = 0;
  }
}

void max7219_modules_driver::set_num_modules(byte a_num_modules)
{
  num_modules = a_num_modules;
  init_command_buffer();
}


byte max7219_modules_driver::get_num_modules()
{
  return num_modules;
}


max7219_modules_driver::max7219_modules_driver()
{
  num_modules = 0;
}


void max7219_modules_driver::command(byte module_offset, byte command, byte data)
{
  if (command_buffer[module_offset].command != CMD_NOOP)
    apply();
  command_buffer[module_offset].command = command;
  command_buffer[module_offset].data = data;
}


void max7219_modules_driver::apply()
{
  spi_channel->announce_transfer();
  for (int i = num_modules - 1; i >= 0; i--)
  {
    spi_channel->transfer_byte(command_buffer[i].command);
    spi_channel->transfer_byte(command_buffer[i].data);
  }
  spi_channel->denounce_transfer();
  /*for (int i = num_modules - 1; i >= 0; i--)
  {
    if ((command_buffer[i].command >= 1) && (command_buffer[i].command <= 8))
    {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(command_buffer[i].command);
      Serial.print("(");
      Serial.print(command_buffer[i].data);
      Serial.print(")");
    }
  }
  Serial.println("");*/
}


void max7219_modules_driver::noop(byte module_offset)
{
  command(module_offset, CMD_NOOP, 0);
}


void max7219_modules_driver::set_intensity(byte module_offset, byte level)
{
  level &= B1111;
  command(module_offset, CMD_SET_INTENSITY, level);
}


void max7219_modules_driver::set_scan_limit(byte module_offset, byte last_row)
{
  last_row &= B111;
  command(module_offset, CMD_SET_SCAN_LIMIT, last_row);
}


void max7219_modules_driver::hide(byte module_offset)
{
  command(module_offset, CMD_SHOW, 0);
}


void max7219_modules_driver::show(byte module_offset)
{
  command(module_offset, CMD_SHOW, 1);
}


void max7219_modules_driver::bcd_mode_on(byte module_offset)
{
  command(module_offset, CMD_SET_BCD, 0);
}


void max7219_modules_driver::bcd_mode_off(byte module_offset)
{
  command(module_offset, CMD_SET_BCD, 1);
}


void max7219_modules_driver::display_test_on(byte module_offset)
{
  command(module_offset, CMD_DISPLAY_TEST, 1);
}


void max7219_modules_driver::display_test_off(byte module_offset)
{
  command(module_offset, CMD_DISPLAY_TEST, 0);
}


void max7219_modules_driver::set_row(byte module_offset, byte row, byte row_bitmask)
{
  row &= B111;
  command(module_offset, 1 + row, row_bitmask);
}
