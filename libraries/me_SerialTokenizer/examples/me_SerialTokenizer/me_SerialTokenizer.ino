// Stream tokenizer demo

/*
  Enter random space/newline separated strings to Serial.

  Non-space sequences are caught by GetEntity() and printed:

    (assuming entity max length of 2):

      "  143 1a la la5" -> "(14) (1a) (la) (la)"
*/

/*
  Author: Martin Eden
  Last mod.: 2024-05-13
*/

#include <me_Types.h>
#include <me_InstallStandardStreams.h>
#include <me_UartSpeeds.h>
#include <me_SerialTokenizer.h>

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  Serial.setTimeout(10);
  InstallStandardStreams();
  printf("[me_SerialTokenizer.ino] Okay, we are here.\n");
}

using namespace me_SerialTokenizer;

void loop()
{
  const TUint_2 EntityMaxLength = 8;
  TChar Entity[EntityMaxLength + 1];

  TUint_2 EntityLength;

  if (GetEntity(&Entity[0], &EntityLength, EntityMaxLength))
  {
    // Add zero byte to entity to make it ASCIIZ (for printf()):
    Entity[EntityLength] = '\0';

    printf("(%s)\n", Entity);
  }
}

/*
  2024-05-08
  2024-05-13
*/
