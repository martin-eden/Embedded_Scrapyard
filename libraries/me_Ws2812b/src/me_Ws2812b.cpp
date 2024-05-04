// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2024-05-04
*/

/*
  Protocol summary

    Packet
      Sequence of colors
      LOW >= 50 us after sending sequence

    Color
      Three bytes
      Order is Green-Red-Blue

    Bits
      Sent from highest to lowest

      SendBit
      ~~~~~~~
        | HIGH
        | Wait_ns(0: 350, 1: 900)
        | LOW
        | Wait_ns(0: 900, 1: 350)

      Bit rate is 800 kBits (1250 ns per bit)
      Temporal encoding (embroidered clock)
        0: 1, 0, 0
        1: 1, 1, 0
*/

#include "me_Ws2812b.h"

#include <stdio.h> // printf()
#include <Arduino.h> // delayMicroseconds()
#include <me_ArduinoUno.h> // PinToIoRegisterAndBit()

// forwards
void EmitBytes(TBytes, TUint_2, TUint_1, TUint_1);

/*
  Send array of bytes
*/
TBool me_Ws2812b::SendBytes(TBytes Bytes, TUint_2 Length, TUint_1 Pin)
{
  TUint_1 PortRegister;
  TUint_1 PortBit;

  // Set <PortRegister> and <PortBit> from <Pin>
  {
    TBool IsOk =
      me_ArduinoUno::PinToIoRegisterAndBit(Pin, &PortRegister, &PortBit);

    if (!IsOk)
    {
      printf("me_Ws2812b: Can't figure out port register for pin %d.\n", Pin);
      return false;
    }
  }

  // Prepare for transmission
  {
    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);
  }

  // Trim <Length> to a multiple of three
  Length = Length - (Length % 3);

  // Transmit
  if (Length > 0)
  {
    EmitBytes(Bytes, Length, PortRegister, PortBit);
  }

  // End transmission
  {
    // Send reset - keep LOW for 50+ us
    const TUint_2 LatchDuration_us = 50;
    delayMicroseconds(LatchDuration_us);
  }

  return true;
}

/*
  Meat function for emitting bytes at 800 kBits
*/
void EmitBytes(
  TBytes Bytes,
  TUint_2 Length,
  TUint_1 PortRegister,
  TUint_1 PortBit
)
{
  TUint_1 DataByte;
  TUint_1 BitCounter;

  /*
    Disable interrupts while sending packet. Or something will happen
    every 1024 us with a duration near 6 us and spoil our signal.

    Interrupt flag is stored among other things in SREG.
  */
  TUint_1 OrigSreg = SREG;
  cli();

  /*
    Double "for" in GNU asm.

    Implementation details

      * Data bytes counter and bits counter are decremented till zero.
        Cleaner assembly code.

      * Bits counter is decremented inside "if"s. We have time slots
        there.

      * "mov <r>, <r>" is used as "nop". Compiler strips "nop"'s
        when optimizing for code size (default "-Os" option).

      * There is no instruction to get bit from by variable index.

        We need bits from highest to lowest, so we can AND with
        0x80 and shift left. But there is better option.

        "lsl" (Logical Shift Left) stores high bit in carry flag and
        shifts left. (Actually it's translated to "add <x>, <x>".)
  */
  asm volatile
  (
    R"(

    # Init

      # Weird instructions to locate this place in disassembly
      ldi %[BitCounter], 0xA9
      ldi %[BitCounter], 0xAA

    DataLoop_Start:

      ld %[DataByte], %a[Bytes]+

      # Eight bits in byte
      ldi %[BitCounter], 8

    BitLoop_Start:

      # For both zero and one bit we first set pin HIGH:
      sbi %[PortRegister], %[PortBit]

      # Get bit
      lsl %[DataByte]

      brcs IsOne

    IsZero:

      # Write LOW, wait ~12 tacts.

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      cbi %[PortRegister], %[PortBit]

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      dec %[BitCounter]
      breq BitLoop_End
      rjmp BitLoop_Start

    IsOne:

      # Wait ~8 tacts, write LOW, wait ~4 tacts.

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      mov %[BitCounter], %[BitCounter]

      cbi %[PortRegister], %[PortBit]

      dec %[BitCounter]
      breq BitLoop_End
      rjmp BitLoop_Start

    BitLoop_End:

    DataLoop_Next:

      sbiw %[RemainedLength], 1
      brne DataLoop_Start

    )"
    :
    // temporary memory
    [RemainedLength] "+w" (Length),
    [DataByte] "=la" (DataByte),
    [BitCounter] "=a" (BitCounter)
    :
    [PortRegister] "i" (PortRegister),
    [PortBit] "i" (PortBit),
    // Pointer to byte array in some auto-incremented register
    [Bytes] "x" (Bytes)
  );

  SREG = OrigSreg;
}

/*
  Send array of pixels
*/
TBool me_Ws2812b::SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin)
{
  // Assert that <Pixels> size is less than 64KiB
  {
    const TUint_2 MaxPixelsLength = 0xFFFF / sizeof(TPixel);

    if (Length > MaxPixelsLength)
    {
      printf(
        "SendPixels(): <Length> is %u and is too long. Max value is %u.\n",
        Length,
        MaxPixelsLength
      );
      return false;
    }
  }

  // Send pixels as bytes
  {
    TBytes* Bytes = (TBytes*) Pixels;
    TUint_2 BytesLength = Length * sizeof(TPixel);

    return SendBytes(*Bytes, BytesLength, Pin);
  }
}

/*
  2024-03 Core
  2024-04 Cleanup
  2024-05 Cleanup
*/