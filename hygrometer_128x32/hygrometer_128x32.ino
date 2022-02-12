// Display humidity and temperature reading on 128x32 display

/*
  Status: stable
  Version: 1.4
  Last mod.: 2022-02-12
*/

/*
  Equipment and wiring

    display: 128x32 ".9 OLED I2C
    hygrometer: DHT11
*/

#include <me_DHT11.h>
#include <U8g2lib.h>

const uint8_t
  Hygrometer_pin = 12;

const float
  MeasurementDelay_sec = 1.0;

me_DHT11 Hygrometer(Hygrometer_pin);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R0);

void setup()
{
  Serial.begin(9600);
  Display.begin();
  Display.setFont(u8g2_font_lubR14_tf);
}

void PrintRawData()
{
  String DataStr = String(Hygrometer.Data[0], HEX) + " ";
  DataStr += String(Hygrometer.Data[1], HEX) + " ";
  DataStr += String(Hygrometer.Data[2], HEX) + " ";
  DataStr += String(Hygrometer.Data[3], HEX) + " ";
  DataStr += String(Hygrometer.Data[4], HEX) + " ";
  Serial.println(DataStr);
}

void DisplayString(String aStr)
{
  char buf[20];
  aStr.toCharArray(buf, 20);

  uint8_t x = 8;
  uint8_t y = 28;

  Display.clearBuffer();
  Display.drawStr(x, y, buf);
  Display.sendBuffer();
}

void loop()
{
  float
    hygrometerHumidity,
    hygrometerTemperature;

  bool result = Hygrometer.Get();
  if (result)
  {
    hygrometerHumidity = Hygrometer.Humidity;
    hygrometerTemperature = Hygrometer.Temperature;
  }
  else
  {
    hygrometerHumidity = -1.0;
    hygrometerTemperature = -128.0;
  }

  String valueStr = String(hygrometerHumidity, 0) + "%";
  valueStr += " " + String(hygrometerTemperature, 1) + "\260C";

  // Serial.println(valueStr);

  DisplayString(valueStr);

  PrintRawData();

  delay(MeasurementDelay_sec * 1000);
}
