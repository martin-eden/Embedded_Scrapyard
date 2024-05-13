// [me_ParseInteger] test/demo

/*
  Enter random space/newline separated strings to Serial.

  Entities that fall into range -32768 .. 32767 are converted to
  integers.

    "  143 1a la" -> (143) '1a'? 'la'?
    "0 -0 +0 --0" -> (0) (0) '+0'? '--0'?
    "000123 0000123 00000123" -> (123) '0000123'? '0000012'?
    "32767 32768" -> (32767) '32768'?
    "-32768 -32769" -> (-32768) '-32769'?

  Under the hood there is also ToUint2() function if you wish
  range 0 .. 65535.
*/

/*
  Author: Martin Eden
  Last mod.: 2024-05-13
*/

#include <me_ParseInteger.h>

#include <me_SerialTokenizer.h>

#include <me_InstallStandardStreams.h>
#include <me_UartSpeeds.h>

#include <me_Types.h>

void setup()
{
  Serial.begin(me_UartSpeeds::Arduino_Normal_Bps);
  Serial.setTimeout(10);
  InstallStandardStreams();
  printf("[me_ParseInteger.ino] Okay, we are here.\n");
}

using namespace me_SerialTokenizer;
using namespace me_ParseInteger;

void loop()
{
  // Length of longest value "-32768"
  const TUint_2 IntegerMaxLength = 6;

  // Length of entity is one more to detect that it won't fit
  const TUint_2 EntityMaxLength = IntegerMaxLength + 1;

  // Memory for entity is one more for zero byte for printf()
  TChar Entity[EntityMaxLength + 1];

  TUint_2 EntityLength;

  if (GetEntity(&Entity[0], &EntityLength, EntityMaxLength))
  {
    // Add zero byte to entity to make it ASCIIZ (for printf()):
    Entity[EntityLength] = '\0';

    // Too long to fit?
    if (EntityLength > IntegerMaxLength)
    {
      printf("'%s'?\n", Entity);
      return;
    }

    TSint_2 IntValue;
    TBool IsConverted;

    IsConverted = ToSint2(&IntValue, Entity, EntityLength);

    if (!IsConverted)
    {
      printf("'%s'?\n", Entity);
      return;
    }

    printf("(%d)\n", IntValue);
  }
}

/*
  2024-05-08
  2024-05-13
*/
