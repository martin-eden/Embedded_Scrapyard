// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2024-05-06
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
*/

/*
  Real world

    What really critical is the length of HIGH pulse for bit 0.
    It should be less than 500 ns. Lengths of LOW strides can
    be quite longer. So code execution overhead for data extraction
    is not critical.
*/

#include "me_Ws2812b.h"

#include <stdio.h> // printf()
#include <Arduino.h> // delayMicroseconds()
#include <me_ArduinoUno.h> // PinToIoRegisterAndBit()

// Summary and forwards
TBool me_Ws2812b::SendPixels(
  TPixel Pixels[],
  TUint_2 Length,
  TUint_1 Pin
);

TBool SendBytes(
  TBytes Bytes,
  TUint_2 Length,
  TUint_1 Pin
);

TBool EmitBytes(
  TBytes Bytes,
  TUint_2 Length,
  TUint_1 Pin
);

/*
  Send array of pixels to given pin

    Length should be less than 21 K.

      (Maximum addressable memory is 64 KiB. Each pixel is 3 bytes.)

      Uno has just 2 KiB memory which is less than 700 pixels.
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
  Send array of bytes to given pin

    Length is trimmed to multiple of three.

      Don't wish to deviate from spec without need.
*/
TBool SendBytes(TBytes Bytes, TUint_2 Length, TUint_1 Pin)
{
  // Trim <Length> to a multiple of three
  Length = Length - (Length % 3);

  // Prepare for transmission
  {
    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);
  }

  TBool Result = true;

  // Transmit
  if (Length > 0)
  {
    Result = EmitBytes(Bytes, Length, Pin);
  }

  // End transmission
  {
    // Send reset - keep LOW for 50+ us
    const TUint_2 LatchDuration_us = 50;
    delayMicroseconds(LatchDuration_us);
  }

  return Result;
}

/*
  Meat function for emitting bytes at 800 kBits
*/
TBool EmitBytes(
  TBytes Bytes,
  TUint_2 Length,
  TUint_1 Pin
)
{
  TUint_2 PortAddress;
  TUint_1 PortAndMask;
  TUint_1 PortOrMask;

  // Populate <PortAddress>, <PortAndMask> and <PortOrMask> from <Pin>
  {
    TUint_1 PortBit;

    TBool IsOk =
      me_ArduinoUno::PinToAddressAndBit(Pin, &PortAddress, &PortBit);

    if (!IsOk)
    {
      printf("me_Ws2812b: Can't figure out port address for pin %d.\n", Pin);
      return false;
    }

    PortOrMask = (1 << PortBit);
    PortAndMask = ~PortOrMask;
  }

  TUint_1 DataByte;
  TUint_1 BitCounter;
  TUint_1 PortValue;

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

      * There are no instructions to get/set bit by variable index.

        Get:

          We need bits from highest to lowest in data byte. So we can
          AND with 0x80 and shift left. But there is better option.

          "lsl" (Logical Shift Left) stores high bit in carry flag and
          shifts left. (Actually it's translated to "add <x>, <x>".)

        Set:

          We need output to specific pin. It means we need to write
          some bit at some byte address.

          We have port address and bit number. We are creating bit masks
          for OR and AND. Reading byte from port at beginning, then
          OR-ing and writing back to set HIGH. AND-ing to set LOW.
  */
  asm volatile
  (
    R"(

    # Init

      # Weird instructions to locate this place in disassembly
      ldi %[BitCounter], 0xA9
      ldi %[BitCounter], 0xAA

      ld %[PortValue], %a[PortAddress]

    DataLoop_Start:

      ld %[DataByte], %a[Bytes]+

      # Eight bits in byte
      ldi %[BitCounter], 8

    BitLoop_Start:

      # Output HIGH
      or %[PortValue], %[PortOrMask]
      st %a[PortAddress], %[PortValue]

      # Extract next data bit
      lsl %[DataByte]

      brcs IsOne

    IsZero:

      # Output LOW
      and %[PortValue], %[PortAndMask]
      st %a[PortAddress], %[PortValue]

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

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]
      mov %[BitCounter], %[BitCounter]

      # Output LOW
      and %[PortValue], %[PortAndMask]
      st %a[PortAddress], %[PortValue]

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
    [PortValue] "=a" (PortValue),
    [BitCounter] "=a" (BitCounter)
    :
    // Pointer to port address
    [PortAddress] "z" (PortAddress),
    [PortAndMask] "la" (PortAndMask),
    [PortOrMask] "a" (PortOrMask),
    // Pointer to byte array in some auto-incremented register
    [Bytes] "x" (Bytes)
  );

  SREG = OrigSreg;

  return true;
}

/*
  2024-03 Core
  2024-04 Cleanup
  2024-05 Core change to support variable pins
*/
