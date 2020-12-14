#include <U8x8lib.h>

U8X8_SH1106_128X64_NONAME_4W_HW_SPI u8x8(/* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

void setup() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.setPowerSave(0);
}

char buf_str[10];

void loop() {
  uint16_t value = analogRead(A0);
  String value_str = String(value);
  value_str.toCharArray(buf_str, 10);

  u8x8.drawString(6, 4, ".....");
  u8x8.drawString(6, 4, buf_str);

  delay(500);
}