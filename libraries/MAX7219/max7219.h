// Driver for MAX7219 8x8-segment LEDs microprocessor

/*
  Status: stable
  Last mod.: 2015-10-14
*/

#pragma once
#include <Arduino.h>
#include "xspi.h"

struct max7219_operation
{
  byte command;
  byte data;
};


class max7219_modules_driver
{
  private:
    byte num_modules;
    void command(byte module_offset, byte command, byte data);
    void init_command_buffer();
  public:
    max7219_operation* command_buffer;
    max7219_modules_driver();
    spi_transciever* spi_channel;
    void set_num_modules(byte a_num_modules);
    byte get_num_modules();
    void apply();
    void noop(byte module_offset);
    void set_intensity(byte module_offset, byte level);
    void set_scan_limit(byte module_offset, byte last_row);
    void hide(byte module_offset);
    void show(byte module_offset);
    void bcd_mode_on(byte module_offset);
    void bcd_mode_off(byte module_offset);
    void display_test_on(byte module_offset);
    void display_test_off(byte module_offset);
    void set_row(byte module_offset, byte row, byte row_bitmask);
};
