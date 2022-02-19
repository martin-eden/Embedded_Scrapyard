#include "me_RandomDots.h"

me_RandomDots::me_RandomDots(
  U8G2* aDisplay,
  uint8_t aLeft = 0,
  uint8_t aTop = 0,
  uint8_t aRight = 127,
  uint8_t aBottom = 31
)
{
  Display = aDisplay;
  Left = aLeft;
  Top = aTop;
  Right = aRight;
  Bottom = aBottom;
}

void me_RandomDots::Draw()
{
  CurrentFillRatio = constrain(CurrentFillRatio, 0, MaxFillRatio);
  for (uint8_t y = Top; y <= Bottom; ++y)
  {
    for (uint8_t x = Left; x <= Right; ++x)
    {
      if ((CurrentFillRatio != 0) && (random(MaxFillRatio + 1) <= CurrentFillRatio))
        Display->setDrawColor(1);
      else
        Display->setDrawColor(0);

      Display->drawPixel(x, y);
    }
  }
}
