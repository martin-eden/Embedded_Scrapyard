#include <me_DequeMind.h>

me_DequeMind Buffer(10);

void setup()
{
  Serial.begin(115200);

  Serial.println("==================================");
  Serial.println("=      Ring buffer test.         =");
  Serial.println("==================================");

  DequeMind_PrintState(&Buffer);

  Buffer.AddBefore();
  DequeMind_PrintState(&Buffer);

  Buffer.AddBefore();
  DequeMind_PrintState(&Buffer);

  for (uint8_t i = 0; i < 7; ++i)
    Buffer.AddAfter();
  DequeMind_PrintState(&Buffer);

  Buffer.AddBefore();
  DequeMind_PrintState(&Buffer);

  Buffer.AddAfter();
  DequeMind_PrintState(&Buffer);

  Buffer.RemoveFirst();
  DequeMind_PrintState(&Buffer);

  Buffer.RemoveLast();
  DequeMind_PrintState(&Buffer);

  for (uint8_t i = 0; i < 7; ++i)
    Buffer.RemoveFirst();
  DequeMind_PrintState(&Buffer);

  Buffer.RemoveFirst();
  DequeMind_PrintState(&Buffer);
}

void loop()
{

}
