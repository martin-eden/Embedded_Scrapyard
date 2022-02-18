#include <U8g2lib.h>
#include <me_128x32_ProgressBar.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);
me_128x32_ProgressBar ProgressBar(&Display, 10, 7, 120, 25);

void setup()
{
  Serial.begin(9600);
  Display.begin();
}

void fillScreen(int16_t fillRatio)
{
  int16_t fillCounter = 0;

  for (uint8_t y = 0; y < 32; ++y)
  {
    for (uint8_t x = 0; x < 128; ++x)
    {
      if (random(1024) <= fillRatio)
        Display.setDrawColor(1);
      else
        Display.setDrawColor(0);

      Display.drawPixel(x, y);
    }
  }
}

void loop()
{
  int16_t lighting = 1023 - analogRead(A0);
  // Serial.println(lightingStr);
  ProgressBar.CurrentValue = lighting;

  Display.clearBuffer();

  fillScreen(lighting);

  ProgressBar.Draw();

  Display.sendBuffer();

  delay(1000);
}
