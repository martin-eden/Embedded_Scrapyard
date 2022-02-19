// Progress bar for monochrome OLED display.

/*
  Status: working prototype
  Version: 0.1
  Last mod.: 2022-02-18
*/

#include "me_ProgressBar.h"

me_ProgressBar::me_ProgressBar(
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

void me_ProgressBar::Draw()
{
  DrawOuterBox();
  DrawFill();
}

void me_ProgressBar::DrawOuterBox()
{
  Display->setDrawColor(1);
  Display->drawFrame(Left, Top, Right - Left + 1, Bottom - Top + 1);
  Display->setDrawColor(0);
  Display->drawBox(Left + 1, Top + 1, Right - 1 - Left - 1 + 1, Bottom - 1 - Top - 1 + 1);
}

void me_ProgressBar::DrawFill()
{
  CurrentValue = constrain(CurrentValue, MinValue, MaxValue);
  uint8_t AwailableWidth = Right - Left - 3;
  uint8_t OccupiedWidth = (float) AwailableWidth * (((float)CurrentValue - MinValue) / MaxValue);
  Display->setDrawColor(1);
  Display->drawBox(Left + 2, Top + 2, OccupiedWidth, Bottom - Top - 3);
}
