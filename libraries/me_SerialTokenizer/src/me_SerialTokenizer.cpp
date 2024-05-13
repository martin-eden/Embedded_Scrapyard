// Getting entity from Serial

/*
  "Entity" is a non-gap sequence surrounded by "gaps".

  "Gap" is space character, newline or "end of stream" condition.
*/

/*
  Author: Martin Eden
  Last mod.: 2024-05-13
*/

#include "me_SerialTokenizer.h"

#include <HardwareSerial.h> // Serial: available(), peek(), getTimeout(), read()
#include <Arduino.h> // delay()
#include <ctype.h> // isspace()

#include <me_Types.h>

using namespace me_SerialTokenizer;

/*
  Get entity from serial stream.

  Arguments

    <@ Entity: u1> - Memory address to store entity bytes
    <@ EntityLengthPtr: u2> - Memory address to store entity size
    <EntityCapacity: u2> - Size of memory to hold entity

  Result

    true - when we got entity

  Notes

    When entity length exceeds capacity, we read until gap and return
    true. Entity is trimmed to capacity.

    We do not write zero byte at end of entity data.

      Bytes between "entity length" and "entity capacity" can
      have any values.
*/
TBool me_SerialTokenizer::GetEntity(
  TChar * Entity,
  TUint_2 * EntityLengthPtr,
  TUint_2 EntityCapacity
)
{
  if (EntityCapacity == 0)
    // you gonna be kidding me!
    return false;

  PurgeSpaces();

  TUint_2 EntityLength;

  EntityLength = 0;

  TChar Char;
  while (PeekCharacter(&Char))
  {
    if (isspace(Char))
      break;

    PurgeCharacter();

    // store character
    *Entity = Char;

    // advance pointer
    Entity = Entity + sizeof(Char);

    // update length
    EntityLength = EntityLength + 1;

    // no place to store?
    if (EntityLength == EntityCapacity)
    {
      PurgeEntity();
      break;
    }
  }
  // Post-condition: we got space or reached end of stream or reached capacity

  if (EntityLength == 0)
    return false;

  // store entity length
  *EntityLengthPtr = EntityLength;

  return true;
}

/*
  Drop characters from stream
  until
    non-space character or
    end of stream
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
  Drop characters from stream
  until
    space character or
    end of stream
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
  2024-05-13
*/
