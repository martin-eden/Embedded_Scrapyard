#include "me_ArduinoUno.h"

/*
  Given pin number return port address and bit.

  These data was used in assembler routine to write to do sorta
  digitalRead()/digitalWrite().
*/
TBool me_ArduinoUno::PinToAddressAndBit(
  TUint_1 Pin,
  TUint_2 *Address,
  TUint_1 *Bit
)
{
  enum PortAddresses:TUint_1
    {
      B = 0x25,
      C = 0x28,
      D = 0x2B
    };

  /*
    Arduino Uno board has 14 "digital" pins (0..13) and
    6 "analog" pins A0 .. A5 (14..19).

    This nomenclature is a bit cursed as "analog" pins can
    do digital input/output and A4..A5 are used for I2C.

    Difference is that "analog" pins can digitize analog signal
    to 10 bits while "digital" pins digitize to one bit.
  */
  if (Pin <= 7)
  {
    *Address = PortAddresses::D;
    *Bit = Pin;
  }
  else if (Pin <= 13)
  {
    *Address = PortAddresses::B;
    *Bit = Pin - 8;
  }
  else if (Pin <= 19)
  {
    *Address = PortAddresses::C;
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
  2024-05-05 [/] to PinToAddressAndBit()
*/
