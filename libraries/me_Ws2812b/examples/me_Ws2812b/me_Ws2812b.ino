/*
  Board: Arduino Uno
*/

// Last mod.: 2024-03-23

#include <me_Types.h>
#include <me_Uart.h>
#include <me_Install_StandardStreams.h>
#include <me_Math.h>

#include <me_Ws2812b.h>

void setup()
{
  const TUint_4 SerialSpeed = me_Uart::Arduino_Normal_Bps;
  Serial.begin(SerialSpeed);
  delay(500);

  Install_StandardStreams();

  printf("[me_Ws2812b.ino]\n");
  printf("Serial speed is %lu bps.\n", SerialSpeed);
  delay(500);
}

void loop()
{
  const TUint_2 LoopDelay_Ms = 30;

  SendTestPacket();

  delay(LoopDelay_Ms);
}

void SendTestPacket()
{
  const TUint_2 NumLeds = 3 * 60;
  TUint_1 TestPacket[NumLeds];
  const TUint_2 TestPacket_Size = sizeof(TestPacket);

  TUint_2 ByteIdx = 0;
  static TUint_2 BaseAngle_Deg = 0;
  TUint_2 Angle_Deg = BaseAngle_Deg;
  while (ByteIdx + 2 < TestPacket_Size)
  {
    TUint_1 EtherValue = (((sin(DegToRad(Angle_Deg)) + 1) / 2 * .6) + .2) * 255;
    // printf("Angle: %u, Ether: %u\n", Angle_Deg, EtherValue);

    TestPacket[ByteIdx] = EtherValue;
    TestPacket[ByteIdx + 1] = EtherValue;
    TestPacket[ByteIdx + 2] = EtherValue;

    Angle_Deg = (Angle_Deg + 6) % 360;
    ByteIdx += 3;
  }
  BaseAngle_Deg = (BaseAngle_Deg + (5)) % 360;

  TestPacket[0] = 0xFF;
  TestPacket[1] = 0x00;
  TestPacket[2] = 0x00;

  TestPacket[TestPacket_Size - 3] = 0x00;
  TestPacket[TestPacket_Size - 2] = 0x00;
  TestPacket[TestPacket_Size - 1] = 0xFF;

  /*
  TUint_1 TestPacket[] =
    {
      0,
      0xFF,
      B00110101
    };
  */

  me_Ws2821b::SendPacket(TestPacket, TestPacket_Size, A1);
}

/*
  2024-03-12
  2024-03-22
  2024-03-23
*/
