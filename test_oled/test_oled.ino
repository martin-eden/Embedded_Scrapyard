#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
typedef u8g2_uint_t u8g_uint_t;

void draw_pixel(void) {
  u8g_uint_t x, y, w2, h2, c, s;
  uint16_t i, cnt = 1;
  w2 = u8g2.getWidth();
  h2 = u8g2.getHeight();
  for (i = 0; i < cnt; i++)
  {
    c = random(128);
    c = (c <= 126 ? c = 0 : c = 1);
    u8g2.setColorIndex(c);

    y = random(h2);
    /*
    s = random(2);
    y = random(sqrt(h2)) * random(sqrt(h2)) / 2;
    if (s == 0)
      y = h2 / 2 - y;
    else
      y = h2 / 2 + y;
    */
    x = random(w2);
    /*
    s = random(2);
    x = random(sqrt(w2)) * random(sqrt(w2)) / 2;
    if (s == 0)
      x = w2 / 2 - x;
    else
      x = w2 / 2 + x;
    */

    u8g2.drawPixel(x, y);
  }
}

uint16_t picture_loop_with_fps(void (*draw_fn)(void)) {
  uint16_t FPS10 = 0;
  uint32_t time;

  // u8g2.clearBuffer();
  draw_fn();
  u8g2.updateDisplay();
  FPS10++;

  return FPS10;
}

void setup(void) {
  u8g2.begin();
  randomSeed(analogRead(A0));
}

void loop(void) {
  uint16_t fps;
  fps = picture_loop_with_fps(draw_pixel);
}
