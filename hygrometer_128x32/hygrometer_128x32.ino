// Display humidity and temperature reading on 128x32 display

/*
  Status: stable
  Version: 1.51
  Last mod.: 2022-02-24
*/

/*
  Equipment and wiring

    Display: 128x32 ".9 OLED, <I2C>
    Hygrometer: DHT11, <Hygrometer_pin>
*/

#include <me_DHT11.h>
#include <U8g2lib.h>

const uint8_t
  Hygrometer_pin = 2;

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

String GetData()
{
  Hygrometer.Get();

  String Result = String(Hygrometer.Humidity, 0) + "%";
  Result += " " + String(Hygrometer.Temperature, 1) + "\260C";

  return Result;
}

String GetRawData()
{
  String Result = String(Hygrometer.Data[0], HEX) + " ";
  Result += String(Hygrometer.Data[1], HEX) + " ";
  Result += String(Hygrometer.Data[2], HEX) + " ";
  Result += String(Hygrometer.Data[3], HEX) + " ";
  Result += String(Hygrometer.Data[4], HEX) + " ";

  return Result;
}

void loop()
{
  float
    hygrometerHumidity,
    hygrometerTemperature;

  String DataStr = GetData();

  DisplayString(DataStr);
  // Serial.println(DataStr);

  String RawDataStr = GetRawData();
  // DisplayString(RawDataStr);
  // Serial.println(RawDataStr);

  delay(MeasurementDelay_sec * 1000);
}
