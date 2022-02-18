#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);

void setup()
{
  // Serial.begin(9600);
  Display.begin();
  Display.setFont(u8g2_font_lubR24_tn);
  Display.setFontMode(1);
}

void fillScreen(int16_t fillRatio)
{
  int16_t fillCounter = 0;

  for (uint8_t y = 0; y < 32; ++y)
  {
    for (uint8_t x = 0; x < 128; ++x)
    {
      /*
      fillCounter += fillRatio;
      // fillCounter += random(-fillRatio / 5, fillRatio / 5);
      if (fillCounter >= 1023)
      {
        fillCounter -= 1023;
        Display.setDrawColor(1);
      }
      else
        Display.setDrawColor(0);
      */
      if (random(1024) <= fillRatio)
        Display.setDrawColor(1);
      else
        Display.setDrawColor(0);

      Display.drawPixel(x, y);
    }
  }
}

void displayString(String aStr)
{
  char buf[20];
  aStr.toCharArray(buf, 20);

  uint8_t x = 28;
  uint8_t y = 28;

  Display.setDrawColor(2);
  Display.drawStr(x, y, buf);
}

void loop()
{
  int16_t lighting = 1023 - analogRead(A0);

  // Serial.println(lightingStr);

  // for (uint16_t lighting = 0; lighting < 1024; ++lighting)
  {
    Display.clearBuffer();
    fillScreen(lighting);
    // String lightingStr = String(lighting);
    // displayString(lightingStr);
    Display.sendBuffer();
    delay(7000);
  }
}
