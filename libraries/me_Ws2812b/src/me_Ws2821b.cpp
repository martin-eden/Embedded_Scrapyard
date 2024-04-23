// Send pixels to LED stripe WS2812B

/*
  Author: Martin Eden
  Last mod.: 2024-04-23
*/

/*
  Protocol

    Summary

      Packet
        Sequence of colors
        Delay >= 50 us after sending sequence

      Color
        Three bytes
        Order is Green-Red-Blue

      Bits
        Sent from highest to lowest
        Bit rate 800 kBits (1250 ns per bit)
        Temporal encoding (embroidered clock)
          0: 1, 0, 0
          1: 1, 1, 0

    Pseudo-code

      SendBytes (Bytes)
      ~~~~~~~~~~~~~~~~~~~

        WHILE (Bytes.BytesRemained >= 3)

          SendByte(Bytes.GetByte())  // green
          SendByte(Bytes.GetByte())  // red
          SendByte(Bytes.GetByte())  // blue

        Wait_us(50)

      SendByte (Byte)
      ~~~~~~~~~~~~~~~

        FOR BitIndex in (7 downto 0)

          Bit = Byte.GetBit(BitIndex)

          Line.SetHigh()
          Wait_ns(Bit == 0: 350, Bit == 1: 900)
          Line.SetLow()
          Wait_ns(Bit == 0: 900, Bit == 1: 350)
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
TBool me_Ws2821b::SendBytes(TBytes Bytes, TUint_2 Length, TUint_1 Pin)
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
  Meat function for emitting bytes at 800 kHz
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

    Interrupt state is stored among other things in SREG.

    1 ms == 100 bytes
    60 leds per meter  => 1.8 ms per meter (~ 500 m/s)
    3 byte per LED
  */
  TUint_1 OrigSreg = SREG;
  cli();

  /*
    Double "for" in GNU asm.

    --

    Implementation details

      --

      We increment bit loop inside "if"s. We have time slots there.

      --

      Thank you Richard Stallman and ArduinoIDE guys for "-Os" default
      option that removes successive "nop"'s from "asm" code. Code is
      so tiny and fast now!

      LED stripe is not controlled but who cares?

      So "andi <r>, 0xFF" is our new nop.

      --

      GNU asm is funny.

      When specifying "x" to hold data in X register pair
      (registers r26, r27) it generates code that uses that
      registers. But if other data is marked as "r" (any register),
      it can freely use r26 for it. Despite it's busy.

      GCC will give warning about undefined code. Generated bad code
      is "ld r26, X+". Yeah, just loading data to register that's
      holding pointer to that data.

      So I've used even more cryptic "la" for data. It means two
      alternatives: place in "l" category or in "a" category.
      "l" is r0 .. r15, "a" is r16 .. r23.

      Also my GNU asm does not support "y" for Y register. "x" and "z"
      are fine but not "y".
  */
  asm volatile
  (
    R"(
      # Weird instructions to locate this place in disassembly
      ldi %[BitCounter], 0xA9
      ldi %[BitCounter], 0xAA

      # DataLoop: Init

    DataLoop_Start:

      ld %[DataByte], %a[Bytes]+

      # BitLoop: Init
      clr %[BitCounter]

    BitLoop_Start:

    # For both zero and one bit we first set pin HIGH:
      sbi %[PortRegister], %[PortBit]

    # Get bit
      /*
        We don't have instruction to get bit from the register using
        variable index.

        So as we need bits from highest to lowest, we can AND with
        0x80 and shift left. But there is better option.

        "lsl" (Logical Shift Left) shifts left and stores high bit
        in carry flag. Actually it translates to "add <x>, <x>".
      */
      lsl %[DataByte]

      brcs IsOne

    # Bit is zero. Write LOW, wait ~12 tacts.
    IsZero:

      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF

      cbi %[PortRegister], %[PortBit]

      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF

      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF

      inc %[BitCounter]
      cpi %[BitCounter], 8

      brsh BitLoop_End

      rjmp BitLoop_Start

    # Bit is one. Wait ~8 tacts, write LOW, wait ~4 tacts.
    IsOne:

      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF

      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF
      andi %[BitCounter], 0xFF

      inc %[BitCounter]
      cpi %[BitCounter], 8

      cbi %[PortRegister], %[PortBit]

      andi %[BitCounter], 0xFF

      brsh BitLoop_End

      rjmp BitLoop_Start

    BitLoop_End:

    DataLoop_Next:

      sbiw %[RemainedLength], 1
      brne DataLoop_Start

    DataLoop_End:
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
TBool me_Ws2821b::SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin)
{
  // Assert that <Pixels> size is less than 64KiB
  {
    const TUint_2 MaxLength = 0xFFFF;
    TUint_2 MaxPixelsLength = MaxLength / sizeof(TPixel);

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
  EmitBytes(): design and implementation thoughts

  1. Naive implementation

    for bytes
      for bits
        if 0
          High(); Wait_ns(350); Low(); Wait_ns(900);
        if 1
          High(); Wait_ns(900); Low(); Wait_ns(350);

    Most likely won't work because of inter-bit and inter-byte delays
    will be over 350 ns. Also you don't have nanoseconds delay.

    Also you have no time control on C level, generating code for if's
    and for for's is compiler's job.

    4 tacts on 16MHz == 250 ns

  2. Chill windows

    1250 ns per bit is 20 ticks.

    Besides flicking output pin, we can squeeze fancy stuff like scaling
    and dithering in that tacts. (I am looking at you, FastLED!)

    But we won't. It is space-time-limited hack and ruins design.

  3. Actually we have more time

    Client is calling SendPixels(), not SendBit().

    We can spend arbitrary long time drinking coffee and preparing
    to send data. Then send it real fast. Then smoke cigarette and
    do cleanup.

  3.1 Prepare direct code

    Can we just convert buffer to AVR instructions and execute them?

    Well, I like this idea but no:

    1. You can't execute memory on AVR. (You can on Intel tho.)
    2. This will bloat memory. 20 ticks for bit means ~ 10-20
      instructions. 10 instructions is like 20 bytes or 160 bits.
      So expansion factor is 160.

      2 KiB memory == 16 KiB bits
      Pixel (3 bytes) == 24 bits
      Expansion factor 160 bits

      So in 2 KiB you can describe 682 LEDs. Or just 4 LEDs if you
      generate direct code.

      Higher clock speeds will increase expansion factor.
    3. But it's paramount in performance. No comparisons, no jumps.

  3.2 Prepare intermediate code

    Can we encode level switches and waits in artificial instructions
    and interpret them fast?

    We can, but it will expand memory again. Let's not create additional
    buffers and just read data by bits. We need assembly level.

  4. Granularity

    Datasheet granularity is three bytes.
    Implementation granularity is one byte.
    Theoretical granularity is one bit.

    So you can send just one byte to see how your LED stripe will
    react. Implementation can be rewritten to send like 5 bits to
    see what will happen.

    But I'm not going to implement bit granularity. My use case is
    sending triples of bytes. Byte granularity was just convenient in
    implementation.
*/

/*
  2024-03 Core
  2024-04 Cleanup
*/
