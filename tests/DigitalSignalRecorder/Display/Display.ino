// Display digital signal recorder signals on 128x32 OLED.

/*
  Status: in writing
  Version: 1.0
  Last mod.: 2022-04-03
*/

/*
  Setup and wiring.

  To test I've used IR receiver module connected to <SignalPin>.
  Pin must support singal change interrupts.
*/

#include <me_DigitalSignalRecorder.h>
#include <me_QueueMindEnumerator.h>
#include <U8g2lib.h>

const uint8_t
  IdleSignalValue = HIGH,
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 38;

me_DigitalSignalRecorder DSR(IdleSignalValue);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R1);

static u8g2_uint_t
  ScreenWidth,
  ScreenHeight;

void setup()
{
  Serial.begin(115200);
  Serial.println("Digital signal recorder OLED display test.");

  if (!DSR.SetCapacity(RecorderCapacity))
  {
    Serial.println("Not enough memory for recorder's buffer.");
    delay(1000);
    exit(1);
  }

  Display.begin();
  ScreenWidth = Display.getWidth(),
  ScreenHeight = Display.getHeight();

  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
}

void OnSignalChange()
{
  DSR.Add(micros(), digitalRead(SignalPin));
}

uint8_t GetMostSignificatByte(uint32_t value)
{
  if (value <= 0xFF)
    return 1;
  else if (value <= 0xFFFF)
    return 2;
  else if (value <= 0xFFFFFF)
    return 3;
  else
    return 4;
}

void DisplaySignals(
  u8g2_uint_t Left,
  u8g2_uint_t Top,
  u8g2_uint_t Width,
  u8g2_uint_t Height
)
{
  const u8g2_uint_t
    RowsPerData = 3,
    MagnitudeWidth = 5;

  Display.clearBuffer();
  me_QueueMindEnumerator Cursor(&DSR.Queue);
  u8g2_uint_t Row = Top;
  do
  {
    uint32_t Pause = DSR.History[Cursor.Get()].Pause;
    uint32_t Signal = DSR.History[Cursor.Get()].Signal;
    u8g2_uint_t SignalPart = (Width - MagnitudeWidth) * Signal / (Pause + Signal);
    Display.drawHLine(Left + MagnitudeWidth, Row, SignalPart);
    u8g2_uint_t MagnitudePart = GetMostSignificatByte(Pause + Signal);
    Display.drawHLine(Left, Row, MagnitudePart);
    Row += RowsPerData;
    if (Row >= Height)
      break;
  } while (Cursor.Move());

  Display.sendBuffer();
}

const uint32_t
  CutoffDelayToPrintMcrs = 1000000;

void loop()
{
  if (
    DSR.IsFull() ||
    (
      DSR.HasEvents() &&
      (micros() - DSR.GetLastEventTime() > CutoffDelayToPrintMcrs)
    )
  )
  {
    detachInterrupt(digitalPinToInterrupt(SignalPin));
    DisplaySignals(0, 0, ScreenWidth, ScreenHeight);
    DSR.Clear();
    attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
  }
  delay(100);
}
