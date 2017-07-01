// SPI implementation header

/*
  Status: stable
  Last mod.: 2015-10-14
*/

/*
  This implementation is used by dot matrix modules under
  control of MAX7219 microprocessor. So other SPI modes
  may be implemented later or never.
*/

#pragma once
#include <Arduino.h>

class spi_transciever
{
  private:
    void transfer_byte_alt(byte data);
  public:
    byte clk_pin;
    byte cs_pin;
    byte out_pin;
    spi_transciever();
    void init_pins();
    void announce_transfer();
    void denounce_transfer();
    void transfer_byte(byte data);
};
