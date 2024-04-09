// Transmit bytes to LED stripe WS2812B

// Author: Martin Eden
// Last mod.: 2024-04-09

/*
  Protocol

    Summary

      Color
        Three bytes
        Order is GRB

      Packet
        Sequence of colors
        Delay >= 50 us after sending packet

      Bits
        Sent from highest to lowest
        Bit rate 800 kBits (1250 ns per bit)
        Temporal encoding (embroidered clock)
          0: 1, 0, 0
          1: 1, 1, 0

    Pseudo-code

      SendBuffer (Buffer)
      ~~~~~~~~~~~~~~~~~~~

        WHILE (Buffer.BytesRemained >= 3)

          SendByte(Buffer.GetByte())  // green
          SendByte(Buffer.GetByte())  // red
          SendByte(Buffer.GetByte())  // blue

        Wait_us(50)


      SendByte (Byte)
      ~~~~~~~~~~~~~~~

        FOR BitIndex in (7 downto 0)

          Bit = Byte.GetBit(BitIndex)

          Line.SetHigh()
          Wait_ns(Bit == 0: 350, Bit == 1: 900)
          Line.SetLow()
          Wait_ns(Bit == 0: 900, Bit == 1: 350)


  Thoughts about implementation design are at the end of file.
*/

#include "me_Ws2812b.h"

#include <stdio.h> // debug print
#include <Arduino.h> // delayMicroseconds() for SendLatch()
#include <me_ArduinoUno.h> // PinToIoRegisterAndBit()

TBool me_Ws2821b::SendPacket(TBytes Bytes, TUint_2 Length, TUint_1 Pin)
{
  TUint_1 PortRegister;
  TUint_1 PortBit;

  {
    TBool IsOk =
      me_ArduinoUno::PinToIoRegisterAndBit(Pin, &PortRegister, &PortBit);

    if (!IsOk)
    {
      printf("me_Ws2821b: Can't figure out port register for pin %d.\n", Pin);
      return false;
    }
  }

  pinMode(Pin, OUTPUT);
  digitalWrite(Pin, LOW);

  if (Length > 0)
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

  {
    const TUint_2 LatchDuration_us = 50;

    delayMicroseconds(LatchDuration_us);
  }

  return true;
}

/*
  Thoughts

  1. Naive implementation

    for bytes
      for bits
        if 0
          ..
        if 1
          ..

    most likely won't work because of inter-bit and inter-byte delays
    will be over 350 ns. Even if it will, we have no time control on
    C level, generating code for if's and for's is compiler's job.

    250 ns is 4 tacts on 16MHz.

  2. Chill windows

    1250 ns per bit is 20 ticks.

    We can squeeze fancy stuff like scaling and dithering in FastLED
    in that tacts.

    But we won't. It is space-limited and ruins design.

  3. Actually we have more time

    Client is calling SendPacket(), not SendBit().

    We can spend arbitrary long time drinking coffee and preparing
    to send data. Then send it real fast. And smoke cigarette and
    do cleanup afterwards.

  3.1 Prepare direct code

    Can we just convert buffer to AVR instructions and execute them?

    Well, I like this idea but no.

    1. You can't execute memory on AVR. You can on Intel tho.
    2. You will bloat memory. 20 ticks for bit means ~ 10-20
      instructions. 10 instructions is like 20 bytes or 160 bits.
      So expansion factor is 160.

      2KiB memory: 2*2^10 * 2^3 == 2^14 == 16386 bits
      LED data: (3 bytes) 3 * 2^3 == 1.5 * 2^4 == 24 bits
      Expansion factor 160: 10 * 2^4 == 1.25 * 2^7 == 160 bits

      So in 2KiB you can describe 682 LEDs. Or just 4 LEDs if you generate
      direct code.

      Higher clock speeds will increase expansion factor.
    3. But it's paramount in performance. No comparisons, no jumps.
      2 MB per second.

  3.2 Prepare intermediate code

    Can we encode level switches and waits in artificial instructions
    and interpret them fast?

    We can, but it will expand memory again. Let's not create additional
    buffers and just read data. We need assembly level.
*/

/*
  Wrapper for sending array of RGB colors.
*/
TBool me_Ws2821b::SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin)
{
  TBytes* Bytes = (TBytes*) Pixels;

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

  TUint_2 BytesLength = Length * sizeof(TPixel);

  return SendPacket(*Bytes, BytesLength, Pin);
}

/*
  2024-03 Core
  2024-04 Cleanup
*/
