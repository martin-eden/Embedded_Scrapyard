// Display humidity and temperature reading on 128x32 display.

/*
  Status: stable
  Version: 2.0
  Last mod.: 2022-06-17
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
  Serial.begin(57600);
  Display.begin();
}

void DisplayOuterFrame()
{
  const u8g2_uint_t
    OuterFrameWidgetX = 0,
    OuterFrameWidgetY = 0,
    OuterFrameWidgetWidth = 128,
    OuterFrameWidgetHeight = 32;

  Display.drawFrame(
    OuterFrameWidgetX,
    OuterFrameWidgetY,
    OuterFrameWidgetWidth,
    OuterFrameWidgetHeight
  );
}

void DisplayGridLines()
{
  const u8g2_uint_t
    GridLinesWidgetX1 = 80,
    GridLinesWidgetX2 = 123;

  Display.drawVLine(GridLinesWidgetX1, 4, 25);
  Display.drawVLine(GridLinesWidgetX2, 4, 25);
}

void DisplayTemperature(float Temperature)
{
  const u8g2_uint_t
    TemperatureWidgetX = 5,
    TemperatureWidgetY = 23;

  char Buffer[8];
  String(String(Temperature, 1) + "\260C").toCharArray(Buffer, sizeof(Buffer));

  Display.setFont(u8g2_font_profont22_tf);
  Display.drawStr(TemperatureWidgetX, TemperatureWidgetY, Buffer);
}

void DisplayHumidity(float Humidity)
{
  const u8g2_uint_t
    HumidityWidgetX = 85,
    HumidityWidgetY = 23;

  char Buffer[8];
  String(String(Humidity, 0) + "%") .toCharArray(Buffer, sizeof(Buffer));

  Display.setFont(u8g2_font_profont22_tf);
  Display.drawStr(HumidityWidgetX, HumidityWidgetY, Buffer);
}

void DisplayFlipFlop()
{
  const u8g2_uint_t
    FlipFlopWidgetX = 125,
    FlipFlopWidgetY = 8,
    FlipFlopWidgetHeight = 19;

  static bool flip = false;
  if (flip)
  {
    // Display.drawVLine(FlipFlopWidgetX, FlipFlopWidgetY, FlipFlopWidgetHeight);
  }

  flip = !flip;
}

void DisplayState()
{
  DisplayOuterFrame();
  DisplayGridLines();
  DisplayTemperature(Hygrometer.Temperature);
  DisplayHumidity(Hygrometer.Humidity);
  DisplayFlipFlop();
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

  Hygrometer.Get();

  DisplayState();

  Display.sendBuffer();

  delay(MeasurementDelay_sec * 1000);
}
