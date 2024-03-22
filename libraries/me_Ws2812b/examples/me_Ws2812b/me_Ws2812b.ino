/*
  Board: Arduino Uno
*/

// Last mod.: 2024-03-22

#include <me_Types.h>
#include <me_Uart.h>
#include <me_Install_StandardStreams.h>

#include <me_Ws2812b.h>

void setup()
{
  const TUint_4 SerialSpeed = me_Uart::Arduino_Normal_Bps;
  Serial.begin(SerialSpeed);
  delay(500);

  Install_StandardStreams();

  printf("Hello there!\nOur serial speed is %lu bps.\n", SerialSpeed);
  delay(500);
}

void loop()
{
  const TUint_2 AnalysisTime_S = 3;

  SendTestPacket();

  delay(AnalysisTime_S * 1000);
}

void SendTestPacket()
{
  const TUint_2 NumLeds = 3 * 60;
  TUint_1 TestPacket[NumLeds];
  const TUint_2 TestPacket_Size = sizeof(TestPacket);

  TUint_2 ByteIdx = 0;
  static TUint_1 Ether = 0;
  while (ByteIdx + 2 < TestPacket_Size)
  {
    TestPacket[ByteIdx] = Ether;
    TestPacket[ByteIdx + 1] = Ether;
    TestPacket[ByteIdx + 2] = Ether;

    Ether = (Ether + 5);
    ByteIdx += 3;
  }

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

  me_Ws2821b::SendPacket(TestPacket, TestPacket_Size);
}

/*
  2024-03-12
  2024-03-22
*/
