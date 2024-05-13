// Parse ASCII to integer value

#pragma once

#include <me_Types.h>

namespace me_ParseInteger
{
  // 0 .. 65535
  TBool ToUint2(
    TUint_2 * ValuePtr,
    TChar * Data,
    TUint_1 DataSize
  );

  // -32768 .. 32767
  TBool ToSint2(
    TSint_2 * ValuePtr,
    TChar * Data,
    TUint_1 DataSize
  );

  // Implementation

  TBool ToDigit(TUint_1 * Digit, TChar Char);
}

/*
  2024-05-13
*/
