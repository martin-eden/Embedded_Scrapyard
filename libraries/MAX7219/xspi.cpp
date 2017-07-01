// SPI implementation

/*
  Status: stable
  Last mod.: 2015-10-14
*/

#include <Arduino.h>
#include "xspi.h"

spi_transciever::spi_transciever()
{
  clk_pin = 3;
  out_pin = 2;
  cs_pin = 4;
}


void spi_transciever::init_pins()
{
  pinMode(clk_pin, OUTPUT);
  pinMode(out_pin, OUTPUT);
  pinMode(cs_pin, OUTPUT);
}


void spi_transciever::announce_transfer()
{
  digitalWrite(cs_pin, LOW);
}


void spi_transciever::denounce_transfer()
{
  digitalWrite(cs_pin, HIGH);
}


void spi_transciever::transfer_byte(byte data)
{
  shiftOut(out_pin, clk_pin, MSBFIRST, data);
  //transfer_byte_alt(data);
}


void spi_transciever::transfer_byte_alt(byte data)
{
  const int low_time = 1;
  const int high_time = 1;
  for (int i = 7; i >= 0; i--)
  {
    digitalWrite(clk_pin, LOW);
    digitalWrite(out_pin, (data >> i) & 1 ? HIGH : LOW);
    delayMicroseconds(low_time);
    digitalWrite(clk_pin, HIGH);
    delayMicroseconds(high_time);
  }
}

