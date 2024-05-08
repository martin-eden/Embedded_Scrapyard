// Getting uint_2 from Serial plus implementation functions

/*
  Author: Martin Eden
  Last mod.: 2024-05-08
*/

#include <me_SerialTokenizer.h>

#include <HardwareSerial.h> // Serial.
#include <Arduino.h> // delay()
#include <ctype.h> // isspace() etc..

using namespace me_SerialTokenizer;

/*
  Get integer in range 0 .. 65535 from ascii stream.

  Stream consumption

    Leading spaces are consumed.

    When character is '0' .. '9'

      Consume all characters until next space (or end of stream)
      If consumed entity is integer in given range
        Fill result and return true

    Else
      We return false

  Beware

    With this logic this function will stuck on stream entries which
    does not start with digit. Like "a".

    It's intended behavior: we can't load integer and will not spoil
    stream.
*/
TBool me_SerialTokenizer::GetUint_2(TUint_2 * Uint_2)
{
  PurgeSpaces();

  if (StreamIsEmpty())
    return false;

  TUint_1 Digit;

  if (!GetDigit(&Digit))
    return false;

  *Uint_2 = Digit;

  const TUint_2 Uint_2_Max_Div10 = 0xFFFF / 10;
  const TUint_1 Uint_2_Max_Mod10 = 0xFFFF % 10;

  while (!StreamIsEmpty())
  {
    /*
      GetDigit() failed. Stream is not empty.
      Stream cursor is on problem character.
      Possible reasons:
        * Space/newline ("1 "). That's okay.
        * Other character ("1a"). That's bad.
    */
    if (!GetDigit(&Digit))
    {
      TChar Char;
      if (!PeekCharacter(&Char))
        // End of stream is not expected but we have result.
        return true;

      // Space after digit is okay
      if (isspace(Char))
        return true;

      // Other symbol after digit is not okay
      PurgeEntity();
      return false;
    }

    // Avoid overflow without casting larger datatype:
    if (
      (*Uint_2 > Uint_2_Max_Div10)
      ||
      (
        (*Uint_2 == Uint_2_Max_Div10) &&
        (Digit > Uint_2_Max_Mod10)
      )
    )
    {
      /*
        Case like "99999" or "65536".

        Even if it's valid digit's sequence, we can't fit it in
        our range.
          We can't unroll stream at this point.
            So purge entity.
      */

      // printf("Overflow imminent! Eject eject EJECT!!1\n");

      PurgeEntity();
      return false;
    }

    *Uint_2 = (*Uint_2) * 10 + Digit;
  }

  return true;
}

/*
  Drop all characters from stream until non-space character.
*/
void me_SerialTokenizer::PurgeSpaces()
{
  TChar Char;
  while (true)
  {
    if (!PeekCharacter(&Char))
      return;

    if (!isspace(Char))
      return;

    PurgeCharacter();
  }
}

/*
  Return next character in stream but do not extract it.

  Implementation will wait for some time if stream is currently empty.
*/
TBool me_SerialTokenizer::PeekCharacter(TChar * Char)
{
  if (StreamIsEmpty())
    delay(Serial.getTimeout());

  return StockPeek(Char);
}

// Return true when serial stream is empty.
TBool me_SerialTokenizer::StreamIsEmpty()
{
  return (Serial.available() == 0);
}

/*
  Return next character in stream but do not consume it.

  Implemented as wrapper over stock Serial.peek().
*/
TBool me_SerialTokenizer::StockPeek(TChar * Char)
{
  TSint_2 PeekResult = Serial.peek();

  // Empty stream?
  if (PeekResult == -1)
    return false;

  *Char = PeekResult;

  // printf("[%c]", *Char);

  return true;
}

/*
  Drop one character from the stream.

  Intended use is to peek character before calling this function.
*/
void me_SerialTokenizer::PurgeCharacter()
{
  if (StreamIsEmpty())
    return;

  Serial.read();
}

/*
  Extract one decimal ascii digit from stream and convert it to 0 .. 9.

  Character is consumed only if it is digit: '0' .. '9'.
*/
TBool me_SerialTokenizer::GetDigit(TUint_1 * Digit)
{
  TChar Char;
  if (!PeekCharacter(&Char))
    return false;

  if (!isdigit(Char))
    return false;

  PurgeCharacter();

  *Digit = Char - '0';

  return true;
}

/*
  Extract one character from the stream.
*/
TBool me_SerialTokenizer::GetCharacter(TChar * Char)
{
  TBool Result;

  Result = PeekCharacter(Char);

  if (Result)
    PurgeCharacter();

  return Result;
}

/*
  Drop all characters from stream until first space.
*/
void me_SerialTokenizer::PurgeEntity()
{
  TChar Char;
  while (true)
  {
    if (!PeekCharacter(&Char))
      return;

    if (isspace(Char))
      return;

    PurgeCharacter();
  }
}

/*
  2024-05-08
*/
