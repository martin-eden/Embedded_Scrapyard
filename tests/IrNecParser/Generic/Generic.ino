// Capturing and parsing IR signals in NEC format.

/*
  Status: stable
  Version: 1.3
  Last mod.: 2022-04-18
*/

#include <ArduinoJson.h>
#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>

using namespace IrNecParser;

me_DigitalSignalRecorder DSR;
me_IrNecParser IrDecoder(&DSR);

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 38;

void setup()
{
  Serial.begin(115200);
  Serial.println("IR NEC protocol decoder test.");

  if (!DSR.SetCapacity(RecorderCapacity))
  {
    Serial.println("Not enough memory for recorder's buffer.");
    delay(1000);
    exit(1);
  }

  pinMode(SignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(SignalPin), OnSignalChange, CHANGE);
}

void OnSignalChange()
{
  DSR.Add(micros(), digitalRead(SignalPin));
}

void loop()
{
  while (IrDecoder.Get())
  {
    PrintStateJson();
  }

  delay(100);
}

void PrintStateJson()
{
  StaticJsonDocument<64> JsonDoc;
  char AddressStr[7];
  char CommandStr[5];

  GetAddressStr(AddressStr, sizeof(AddressStr));
  GetCommandStr(CommandStr, sizeof(CommandStr));

  JsonDoc["Address"] = AddressStr;
  JsonDoc["Command"] = CommandStr;
  JsonDoc["HasShortRepeat"] = IrDecoder.HasShortRepeat;
  JsonDoc["IsRepeat"] = IrDecoder.IsRepeat;

  serializeJsonPretty(JsonDoc, Serial);
  Serial.println();
}

void GetAddressStr(char* Buffer, uint16_t BufferSize)
{
  snprintf(Buffer, BufferSize, "0x%04X", IrDecoder.Address);
}

void GetCommandStr(char* Buffer, uint16_t BufferSize)
{
  snprintf(Buffer, BufferSize, "0x%02X", IrDecoder.Command);
}
