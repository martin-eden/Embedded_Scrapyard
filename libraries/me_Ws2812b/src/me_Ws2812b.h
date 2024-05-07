// Send data to WS2812B LED stripe

/*
  Author: Martin Eden
  Last mod.: 2024-05-06
*/

/*
  Send pixels

    SendPixels
      -<
        Pixels[] : TPixel
        Length : u2
          Should be less than 21K.
        OutputPin : u1
      -> bool

  Pixel definition

    TPixel
      Green : ui1
      Red : ui1
      Blue : ui1
*/

#pragma once

#include <me_Types.h>

namespace me_Ws2812b
{
  struct TPixel
  {
    TUint_1 Green;
    TUint_1 Red;
    TUint_1 Blue;
  };

  TBool SendPixels(TPixel Pixels[], TUint_2 Length, TUint_1 Pin);
}

/*
  2024-03 Core
  2023-05 Cleanup
*/
