/*
  Board: Arduino Uno
*/

// Last mod.: 2024-03-16

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
  const TUint_2 AnalysisTime_S = 0;

  SendTestPacket();

  delay(AnalysisTime_S * 1000);
}

void SendTestPacket()
{
  TUint_1 TestPacket[] =
    {
      0,
      0xFF,
      B00110101
    };

  me_Ws2821b::SendPacket(TestPacket, sizeof(TestPacket));
}

/*
  2024-03-12
*/
