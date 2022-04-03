// Digital signale recorder library generic test setup

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-04-02
*/

/*
  Setup and wiring.

  To test I've used IR receiver module connected to <SignalPin>.
  Pin must support singal change interrupts.
*/

#include <me_DigitalSignalRecorder.h>
#include <me_StatePrinters_DigitalSignalRecorder.h>

const uint8_t
  IdleSignalValue = HIGH,
  SignalPin = 2; // 2 or 3 for ATmega328P

const uint16_t
  RecorderCapacity = 100;

me_DigitalSignalRecorder DSR(IdleSignalValue);

void setup()
{
  Serial.begin(115200);
  Serial.println("Digital signal recorder test.");

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
    noInterrupts();
    DSR_PrintJSON(&DSR);
    DSR.Clear();
    interrupts();
  }
  delay(100);
}
