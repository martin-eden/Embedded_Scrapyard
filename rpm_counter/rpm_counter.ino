// Counting by ISP

/*
  Status: stable
  Last mod.: 2022-08-19
*/

/*
  Used to count rotations of DC motor.

  (Small magnet was attached to wheel and Hall sensor was used.)
*/

#include <U8g2lib.h>

const uint32_t
  MeasurementsIntervalMs = 60000;

const uint8_t
  SensorPin = 2; // should support hardware interrupts, pin 2 or 3 for Uno

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

void setup()
{
  pinMode(SensorPin, INPUT);

  Serial.begin(57600);

  Display.begin();
  Display.setContrast(10);
  Display.setFont(u8g2_font_profont22_tf);
}

uint32_t Counter = 0;

void ConnectCounter()
{
  attachInterrupt(digitalPinToInterrupt(SensorPin), CounterISP, FALLING);
}

void DisconnectCounter()
{
  detachInterrupt(digitalPinToInterrupt(SensorPin));
}

void CounterISP()
{
  Counter++;
}

void SendState()
{
  Serial.print(Counter);
  Serial.print("\n");
}

void DisplayState()
{
  const u8g2_uint_t
    WidgetX = 18,
    WidgetY = 23;

  Display.clearBuffer();

  char Buffer[10];
  String(String(Counter)).toCharArray(Buffer, sizeof(Buffer));
  Display.drawStr(WidgetX, WidgetY, Buffer);

  Display.sendBuffer();
}

void loop()
{
  DisconnectCounter();
  SendState();
  DisplayState();
  ConnectCounter();
  delay(MeasurementsIntervalMs);
}
