#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);

void setup()
{
  Display.begin();
}

void fillScreen()
{
  Display.clearBuffer();
  for (uint8_t x = 0; x < 128; ++x)
  {
    for (uint8_t y = 0; y < 32; ++y)
    {
      if ((32 * y + x) % 3 == 0)
        Display.drawPixel(x, y);
    }
  }
  Display.sendBuffer();
}

void clearScreen()
{
  Display.clearBuffer();
  Display.drawBox(0, 0, 128, 32);
  Display.sendBuffer();
}

void loop()
{
  fillScreen();
  delay(2000);
  // clearScreen();
  // delay(1500);
}
