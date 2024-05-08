// Get uint_2 from Serial plus implementation functions

#pragma once

#include <me_Types.h>

namespace me_SerialTokenizer
{
  TBool StreamIsEmpty();
  TBool StockPeek(TChar * Char);
  TBool PeekCharacter(TChar * Char);

  void PurgeCharacter();
  void PurgeSpaces();
  void PurgeEntity();

  TBool GetDigit(TUint_1 * Digit);
  TBool GetCharacter(TChar * Char);

  TBool GetUint_2(TUint_2 * Uint_2);
}

/*
  2024-05-08
*/
