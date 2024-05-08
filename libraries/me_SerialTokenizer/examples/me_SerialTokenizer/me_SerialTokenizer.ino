// Designing stream tokenizer

/*
  Author: Martin Eden
  Last mod.: 2024-05-08
*/

#include <me_Types.h>
#include <me_Install_StandardStreams.h>
#include <me_UartSpeeds.h>
#include <me_SerialTokenizer.h>

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  Serial.setTimeout(30);
  Install_StandardStreams();
  printf("[SerialTokenizer.ino] Okay, we are here.\n");
}

using namespace me_SerialTokenizer;

void loop()
{
  while (StreamIsEmpty())
    delay(100);
  delay(50);
  // Post-condition: we gave enough time for Stream to receive data

  /*
    On Uno in Stream implementation buffer size is 64 bytes.
    So ("0"<*63> "1") is parsed like 1, but not ("0" <*64> "1").
    Streaming data is bad idea in my Arduino experience.

    General rule is that we are sending packets less than
    64 bytes each. And doing pauses between them. Or waiting
    acknowledge from Uno.
  */

  TUint_2 Uint_2;
  if (GetUint_2(&Uint_2))
  {
    printf("(%u)\n", Uint_2);
  }
  else {
    /*
      GetUint_2 may or may not consume entity.
        It consumes entity when it starts with digit.
          And leaves cursor at end of stream or on space.
    */
    TChar Char;
    if (PeekCharacter(&Char))
    {
      if (!isspace(Char))
      {
        // Entity was not consumed
        printf("'%c'..?\n", Char);
        PurgeEntity();
      }
    }
  }
}

/*
  2024-05-08
*/
