#include "me_ArduinoUno.h"

TBool me_ArduinoUno::PinToIoRegisterAndBit(
  TUint_1 Pin,
  TUint_1 *Register,
  TUint_1 *Bit
)
{
  /*
    "IO register addresses" are memory addresses 0x20..0x3F decreased
    by 0x20.

    That's because AVR have SetBit/ClearBit (sbi/cbi) instructions but
    they require 5-bit addresses. (Can't fit more in 16-bit format.)
    And the first 0x20 bytes of memory are CPU registers.

    So they sacrificed clarity for performance. Flipping output level
    every CPU tick is cool tho.
  */

  enum IoPortRegisters:TUint_1
    {
      B = 0x05,
      C = 0x08,
      D = 0x0B
    };

  /*
    Arduino Uno board has 14 "digital" pins (0..13) and
    6 "analog" pins A0 .. A5 (14..19).

    This nomenclature is a bit cursed as "analog" pins can
    do digital input/output and A4..A5 are used for I2C.

    Difference is that "analog" pins can digitize analog signal
    to 10 bits while "digital" pins digitize signal to one bit.
  */
  if (Pin <= 7)
  {
    *Register = IoPortRegisters::D;
    *Bit = Pin;
  }
  else if (Pin <= 13)
  {
    *Register = IoPortRegisters::B;
    *Bit = Pin - 8;
  }
  else if (Pin <= 19)
  {
    *Register = IoPortRegisters::C;
    *Bit = Pin - 14;
  }
  else
  {
    return false;
  }

  return true;
}

/*
  2024-03-23 PinToIoRegisterAndBit()
*/
