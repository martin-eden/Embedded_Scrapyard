// Send data to WS2812B LED stripe

// Author: Martin Eden
// Last mod.: 2024-04-09

/*
  SendPacket

    Send bytes. Up to 64 KiB.

    (ui1 Bytes[], ui2 Length, ui1 OutputPin) -> bool

  SendPixels

    Send pixels. Up to ~21K.

    ((3*ui1) Pixels[], ui2 Length, ui1 OutputPin) -> bool
*/

#pragma once

#include <me_Types.h>

namespace me_Ws2821b
{
  TBool SendPacket(TBytes Bytes, TUint_2 Length, TUint_1 Pin);

  struct TPixel
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  TBool SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin);
}
