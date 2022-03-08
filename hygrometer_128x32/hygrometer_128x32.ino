// Display humidity and temperature reading on 128x32 display

/*
  Status: stable
  Version: 1.7
  Last mod.: 2022-03-08
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
  Display.setFont(u8g2_font_ncenB24_tr);
}

void InvertDisplayColors()
{
  Display.sendF("c", 0x0a7);
}

void ResetDisplayColors()
{
  Display.sendF("c", 0x0a6);
}

void DisplayState()
{
  Display.setDrawColor(1);

  const uint8_t CharBufSize = 10;
  char buf[CharBufSize];

  String(Hygrometer.Humidity, 0).toCharArray(buf, CharBufSize);
  Display.drawStr(3, 30, buf);

  String(Hygrometer.Temperature, 1).toCharArray(buf, CharBufSize);
  Display.drawStr(58, 26, buf);

  HeartBeat();
}

void HeartBeat()
{
  static bool flip = false;
  Display.setDrawColor(flip ? 0 : 1);
  Display.drawDisc(49, 16, 3);
  flip = !flip;
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
  Display.clearBuffer();

  String DataStr = GetData();

  DisplayState();
  // Serial.println(DataStr);

  String RawDataStr = GetRawData();
  // DisplayState(RawDataStr);
  // Serial.println(RawDataStr);

  Display.sendBuffer();

  delay(MeasurementDelay_sec * 1000);
}
