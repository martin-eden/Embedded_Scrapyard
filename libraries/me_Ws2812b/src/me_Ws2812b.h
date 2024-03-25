#pragma once

#include <me_Types.h>

namespace me_Ws2821b
{
  TBool SendPacket(TBytes Bytes, TUint_2 Length, TUint_1 Pin);

  struct TColor
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  typedef TColor TPixel;

  TBool SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin);
}
