// Capturing and parsing IR signals in NEC format.

/*
  Status: works
  Version: 1.0
  Last mod.: 2022-03-15
*/

#include <me_DigitalSignalRecorder.h>
#include <me_IrNecParser.h>

me_DigitalSignalRecorder DSR;
me_IrNecParser IrDecoder(&DSR);

const uint8_t
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 40;

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
  if (IrDecoder.Get())
  {
    String IrData = "";
    String AddrStr = String(IrDecoder.Address, HEX);
    AddrStr.toUpperCase();
    AddrStr = String("0x") + AddrStr;
    String CmdStr = String(IrDecoder.Command, HEX);
    CmdStr.toUpperCase();
    CmdStr = String("0x") + CmdStr;
    IrData +=
      "{\"address\": " + AddrStr + ", " +
      "\"command\": " + CmdStr + "}";
    Serial.println(IrData);
  }
  delay(100);
}
