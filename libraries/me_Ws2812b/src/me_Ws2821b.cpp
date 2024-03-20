// Send bytes array according to WS2821B specs. (LED stripe)

/*
  Status

    Correct in specific cases.

    Viewed compiled code with disassembler.

    Tested pin state changes with oscilloscope.

    So core part is done. But it receives <Length> in byte. <Length>
    should be word. So it can send up to 200 bytes, not 2k. Need to
    fix it.

    2024-03-17
*/

// Author: Martin Eden
// Last mod.: 2024-03-19

#include "me_Ws2812b.h"

#include <stdio.h> // debug print
#include <Arduino.h> // delayMicroseconds()


/*
  Protocol

    send_buffer(buf)

      while has_over_three_bytes(buf)
        send_byte(green = get_byte(buf))
        send_byte(blue = get_byte(buf))
        send_byte(red = get_byte(buf))

    send_byte(byte)

      for bit_idx = 7, 0
        send_bit(get_bit(byte, bit_idx))

    send_bit(bit)

      (bit == 0):
        line(HIGH)
        wait_ns(350)
        line(LOW)
        wait_ns(900)

      (bit == 1):
        line(HIGH)
        wait_ns(900)
        line(LOW)
        wait_ns(350)

      // 800 kBits rate

  Thoughts about implementation design are at the end of file.
*/

// using namespace me_Ws2821b;

// Forward declarations (
void SendLatch();
// )

void me_Ws2821b::SendPacket(TBytes Bytes, TUint_2 Length)
{
  /*
  printf("Whew! Got your data.\nLength: %u\n", Length);
  for (TUint_1 Key = 0; Key < Length; ++Key)
  {
    TUint_1 Value = Bytes[Key];
    printf("[%u] = %u\n", Key, Value);
  }
  */

  /*
  for (TUint_1 Index = 0; Index < Length; ++Index)
  {
    TUint_1 Value = Bytes[Index];
    SendByte(Value);
  }
  */
  {
    // For initial test we will use Led13. For next test we will use A0.
    // Led13: Port B, Bit 5
    // A0: Port C, Bit 0 == I/O register 0x08, bit 0

    TUint_1 Pin = A0;

    pinMode(Pin, OUTPUT);
    digitalWrite(Pin, LOW);

    TUint_2 DataCounter;
    TUint_1 DataByte;
    TUint_1 BitCounter;

    /*
      Double "for" in GNU asm.

      We iterate over byte array and over 8 bits in byte. From highest
      bit to lowest.

      For each bit we do

        SetPin(HIGH)
        Delays_ns[BitValue].AfterHigh
        SetPin(LOW)
        Delays_ns[BitValue].AfterLow

      In implementation we can "SetPin(HIGH)" early.
      --
      Thank you Richard Stallman and ArduinoIDE guys for "-Os"
      default option that removes successive "nop"'s from "asm" code.

      Code is so tiny and fast now!

      LED stripe is not controlled but who cares?

      So "andi <r>, 0xFF" is a new nop. Until those jerks will
      "optimize" it.
    */
    /*
      To do: main problem is interbyte delay.
        Remove <DataCounter>
        Decrement <Length> until zero (sbiw).
        Rename <Length> to <RemainedLength>.
    */
    asm volatile
    (
      R"(
        # Weird instructions to locate this place in disassembly
        ldi %[DataCounter], 0xA9
        ldi %[BitCounter], 0xAA

        # DataLoop: Init
        clr %A[DataCounter]
        clr %B[DataCounter]

      DataLoop_Start:

        # DataLoop: Check low byte
        cp %A[DataCounter], %A[Length]
        brne DataLoop_Body_Start

        # DataLoop: Check high byte
        cp %B[DataCounter], %B[Length]
        breq DataLoop_End

      DataLoop_Body_Start:

        ld %[DataByte], %a[Bytes]+

        # BitLoop: Init
        clr %[BitCounter]

      BitLoop_Start:

        # 8 bits in byte
        cpi %[BitCounter], 8
        brsh BitLoop_End

      # For both zero and one bit we first set pin HIGH:
        sbi 0x08, 0

      # Get bit
        /*
          We can't get bit from the register using variable index.

          That is why in C variable is ANDed with 1 and then shifted
          right.

          But we need to go from highest bit to lowest, so we will
          get highest bit and shift left.

          "lsl" (Logical Shift Left) shifts left and stores high bit
          in carry flag. Actually it translates to "add <x>, <x>".
        */
        lsl %[DataByte]

        brcs IsOne

      # Bit is zero. Write LOW, wait ~12 tacts.
      IsZero:

        # andi r20, 0xFF

        cbi 0x08, 0

        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF

        # andi r20, 0xFF
        # andi r20, 0xFF

        rjmp BitLoop_Next

      # Bit is one. Wait ~8 tacts, write LOW, wait ~4 tacts.
      IsOne:

        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF

        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF
        # andi r20, 0xFF

        # andi r20, 0xFF

        cbi 0x08, 0

      BitLoop_Next:

        inc %[BitCounter]

        rjmp BitLoop_Start

      BitLoop_End:

      DataLoop_Next:

        # DataLoop: Increment low byte
        inc %A[DataCounter]
        brne DataLoop_Body_End

        #DataLoop: Increment high byte
        inc %B[DataCounter]

      DataLoop_Body_End:

        rjmp DataLoop_Start

      DataLoop_End:
      )"
      :
      // temporary memory
      [DataCounter] "=r" (DataCounter),
      [DataByte] "=r" (DataByte),
      [BitCounter] "=r" (BitCounter)
      :
      // Pointer to byte array in X register. (X is a register pair r27:r26.)
      [Bytes] "x" (Bytes),
      [Length] "r" (Length)
    );
  }

  SendLatch();
}

void SendLatch()
{
  /*
  {
    printf("SendLatch()\n");
  }
  //*/

  const TUint_2 LatchDuration_us = 50;

  delayMicroseconds(LatchDuration_us);
}

/*
  Thoughts

  1. Naive implementation won't work

    250 ns is 4 tacts on 16MHz.

    and we need to do these actions in these 4 tacts:

      return from wait_ns()
      return from send_bit()
      return from send_byte()
      fetch next data byte
      calling set_byte()

  2. Chill windows

    1250 ns is 5 * 250 ns time slots.

    Transfer 0:

      1 1, set 1
      2 1/0, set 0
      3 0, can chill here
      4 0, can chill here
      5 0, return

    Transfer 1:

      1 1, set 1
      2 1, can chill here
      3 1, can chill here
      4 1/0, set 0
      5 0, return

    So we have at least 8 ticks to do some fancy stuff like dithering in
    FastLED. But we won't.

  3. Actually we have more time

    Client is calling send_buffer(). Not send_bit().

    We can spend arbitrary long time drinking coffee and preparing
    to send buffer, sending it real fast and smoking cigarette
    afterwards.

  3.1 Prepare direct code

    Can we just convert buffer to AVR instructions and execute them?

    Well, I like this idea but no.

    1. You can't execute memory on AVR. You can on Intel tho.
    2. You will bloat memory. 20 ticks for bit means ~ 10-20
      instructions. 10 instructions is like 20 bytes or 160 bits.
      So expansion factor is 160.

      2KiB memory in bits: 2*2^10 * 2^3 == 2^14
      LED data in bits: (3 bytes) 3 * 2^3 == 1.5 * 2^4
      Expansion factor 160 in bits: 10 * 16 == 10 * 2^4 == ... == 1.25 * 2^7

      So in 2KiB you can fit 682 LEDs. Or just 4 LEDs if you generate
      direct code.

      Higher clock speeds will increase expansion factor.

  3.2 Prepare intermediate code

    Can we encode level switches and waits in artificial instructions
    and interpret them fast?

    We can, but it expand memory again. Let's not create additional
    buffers and just read data. We need assembly level.
*/
