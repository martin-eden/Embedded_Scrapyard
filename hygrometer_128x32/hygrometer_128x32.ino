// Display humidity and temperature reading on 128x32 display

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-02-04
*/

/*
  Equipment and wiring

    display: 128x32 ".9 OLED I2C
    hygrometer: DHT11
*/

#include <SimpleDHT.h>
#include <U8g2lib.h>

const uint8_t
  Hygrometer_pin = 12;

const float
  MeasurementDelay_sec = 0.5;

SimpleDHT11 Hygrometer(Hygrometer_pin);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);

void setup()
{
  Serial.begin(9600);
  Display.begin();
  Display.setFont(u8g2_font_luIS19_tf);
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

void loop()
{
  float
    hygrometerTemperature,
    hygrometerHumidity;

  int16_t result = Hygrometer.read2(&hygrometerTemperature, &hygrometerHumidity, NULL);
  if (result != SimpleDHTErrSuccess) {
    hygrometerTemperature = -128.0;
    hygrometerHumidity = -1.0;
  }

  String value_str = String(hygrometerHumidity, 0) + "%";
  value_str += " " + String(hygrometerTemperature, 0) + "\260C";

  // Serial.println(value_str);

  char buf_str[20];
  value_str.toCharArray(buf_str, 20);

  uint8_t x = 8;
  uint8_t y = 28;

  Display.clearBuffer();
  Display.drawStr(x, y, buf_str);
  Display.sendBuffer();

  delay(MeasurementDelay_sec * 1000);
}
