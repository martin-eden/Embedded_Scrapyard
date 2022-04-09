// Capturing and parsing IR signals in NEC format.

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-04-02
*/

#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>

using namespace IrNecParser;

me_DigitalSignalRecorder DSR;
me_IrNecParser IrDecoder(&DSR);

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 36;

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

char Buffer[100];

void loop()
{
  if (IrDecoder.Get())
  {
    sprintf(
      Buffer,
      "{"
      "\"address\": \"0x%04X\", "
      "\"command\": \"0x%02X\", "
      "\"hasShortRepeat\": %u, "
      "\"numRepeats\": %u"
      "}",
      IrDecoder.Address,
      IrDecoder.Command,
      IrDecoder.HasShortRepeat,
      IrDecoder.NumRepeats
    );
    Serial.println(Buffer);
  }

  delay(100);

  /*
  static uint32_t LastTime = millis;
  if (millis() - LastTime > 5000)
  {
    Serial.println("Still alive.");
    LastTime = millis();
  }
  */

}
