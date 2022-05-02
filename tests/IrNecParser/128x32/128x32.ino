// Display IR NEC format signal data.

/*
  Status: stable
  Version: 1.25
  Last mod.: 2022-05-01
*/

#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>
#include <me_StatePrinters_IrNecParser_128x32.h>
#include <U8g2lib.h>

using namespace IrNecParser;

me_DigitalSignalRecorder DSR;
me_IrNecParser IrDecoder(&DSR);
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C Display(U8G2_R2);

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

bool
  SignalCaptureEnabled = false;

volatile uint32_t
  LastSignalTime = 0;

const uint16_t
  RecorderCapacity = 38;

void setup()
{
  Serial.begin(115200);
  Serial.println("IR NEC protocol decoder test.\nDisplays results on 128x32 OLED display.");

  Display.begin();
  Display.setContrast(10);

  if (!DSR.SetCapacity(RecorderCapacity))
  {
    Serial.println("Not enough memory for recorder's buffer.");
    delay(1000);
    exit(1);
  }

  InitSignalCapture();

  DisplayIntro();

  EnableSignalCapture();
}

void InitSignalCapture()
{
  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
}

void OnSignalChange()
{
  if (SignalCaptureEnabled)
  {
    uint32_t CurrentTime = micros();
    DSR.Add(CurrentTime, digitalRead(SignalPin));
    LastSignalTime = CurrentTime;
  }
}

void EnableSignalCapture()
{
  SignalCaptureEnabled = true;
}

void DisableSignalCapture()
{
  SignalCaptureEnabled = false;
}

void loop()
{
  static bool IsDisplayingIntro = true;
  static uint32_t LastDisplayTimeMs = 0;

  uint32_t TimeSinceLastSignalMs = (micros() - LastSignalTime) / 1000;
  if (DSR.IsFull() || ((TimeSinceLastSignalMs >= 120) && (DSR.HasEvents())))
  {
    DisableSignalCapture();
    while (IrDecoder.Get())
    {
      IsDisplayingIntro = false;

      Display.clearBuffer();
      IrNec_DisplayState(&IrDecoder, &Display);
      Display.sendBuffer();

      LastDisplayTimeMs = millis();
    }
    DSR.Clear();
    EnableSignalCapture();
  }

  uint32_t TimeSinceLastDisplayS = (millis() - LastDisplayTimeMs) / 1000;
  if (!IsDisplayingIntro && (TimeSinceLastDisplayS >= 40))
  {
    DisplayIntro();
    IsDisplayingIntro = true;
  }

  delay(5);
}

void DisplayIntro()
{
  Display.clearBuffer();
  Display.setFont(u8g2_font_commodore64_tr);
  drawStrCentered(12, "IR NEC");
  drawStrCentered(29, "parser");
  Display.sendBuffer();
}

void drawStrCentered(uint16_t y, const char* s)
{
  uint16_t x = GetCenteredStringX(s);
  Display.drawStr(x, y, s);
}

uint16_t GetCenteredStringX(const char* s)
{
  return (Display.getWidth() - Display.getStrWidth(s)) / 2;
}
