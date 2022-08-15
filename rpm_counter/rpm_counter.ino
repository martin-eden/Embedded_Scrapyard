// Approximating rpm by counting peaks via ISR

/*
  Status: ok
  Last mod.: 2022-08-13
*/

#include <U8g2lib.h>

const uint32_t measure_delay_ms = 60000;

const uint8_t sensor_pin = 2; // hard-wired for Uno, other values won't work
const uint8_t interrupt_number = 0; // depends on <sensor_pin>

volatile uint32_t rpm_counter = 0;

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

void pin_handler()
{
  rpm_counter++;
}

void setup()
{
  pinMode(sensor_pin, INPUT);
  attachInterrupt(interrupt_number, pin_handler, FALLING);

  Serial.begin(57600);

  Display.begin();
  Display.setContrast(10);
  Display.setFont(u8g2_font_profont22_tf);
}

void loop()
{
  Serial.print(rpm_counter);
  Serial.print("\n");

  Display.clearBuffer();

  const u8g2_uint_t
    WidgetX = 18,
    WidgetY = 23;

  char Buffer[8];
  String(String(rpm_counter)).toCharArray(Buffer, sizeof(Buffer));
  Display.drawStr(WidgetX, WidgetY, Buffer);

  Display.sendBuffer();

  delay(measure_delay_ms);
}
