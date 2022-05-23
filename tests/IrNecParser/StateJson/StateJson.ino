// Displays IR NEC parser state in JSON format.

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-05-17
*/

#include <ArduinoJson.h>
#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>
#include <me_StateGetters_IrNecParser.h>

me_DigitalSignalRecorder DSR;
me_IrNecParser::Parser IrDecoder(&DSR);

const uint16_t
  BaudRate = 115200;

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 40;

uint32_t
  LastSignalTime = 0;

bool
  SignalCaptureEnabled = false;

void setup()
{
  Serial.begin(BaudRate);
  while (!Serial.available())
    delay(25);
  Serial.println("IR NEC protocol decoder test.");

  if (!DSR.SetCapacity(RecorderCapacity))
  {
    Serial.println("Not enough memory for recorder's buffer.");
    delay(1000);
    exit(1);
  }

  InitSignalCapture();
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
  uint32_t TimeSinceLastSignalMs = (micros() - LastSignalTime) / 1000;
  if (DSR.IsFull() || ((TimeSinceLastSignalMs >= 120) && (DSR.HasEvents())))
  {
    DisableSignalCapture();
    while (IrDecoder.Get())
    {
      PrintStateJson();
    }
    DSR.Clear();
    EnableSignalCapture();
  }
  delay(10);
}

void PrintStateJson()
{
  using namespace me_IrNecParser_StateGetter;
  ParserState parserState;

  GetState(&IrDecoder, parserState);

  StaticJsonDocument<32> JsonDoc;

  JsonDoc["Address"] = parserState.Address;
  JsonDoc["Command"] = parserState.Command;
  JsonDoc["HasShortRepeat"] = parserState.HasShortRepeat;
  JsonDoc["IsRepeat"] = parserState.IsRepeat;

  serializeJson(JsonDoc, Serial);
  Serial.println();
}
