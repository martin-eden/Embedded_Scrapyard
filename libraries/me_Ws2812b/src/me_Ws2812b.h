// Send data to WS2812B LED stripe

/*
  Author: Martin Eden
  Last mod.: 2024-04-23
*/

/*
  SendPacket

    Send bytes. Up to 64 KiB.

    (u1 Bytes[], u2 Length, u1 OutputPin) -> bool

  SendPixels

    Send pixels. Up to ~21K.

    ((3*u1) Pixels[], u2 Length, u1 OutputPin) -> bool
*/

#pragma once

#include <me_Types.h>

namespace me_Ws2812b
{
  TBool SendBytes(TBytes Bytes, TUint_2 Length, TUint_1 Pin);

  struct TPixel
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  TBool SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin);
}
