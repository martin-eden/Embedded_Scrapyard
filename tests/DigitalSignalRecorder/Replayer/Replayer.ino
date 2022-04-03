// Digital signal replayer test

/*
  Status: still experimenting, memory issues possible
  Version: 1.0
  Last mod.: 2022-04-03
*/

/*
  Setup and wiring.

  To test I've used IR receiver module connected to <InputSignalPin>.
  Pin must support singal change interrupts. For output connect
  IR emitter to <OutputSignalPin>.
*/

#include <me_DigitalSignalRecorder.h>
#include <me_StatePrinters_DigitalSignalRecorder.h>
#include <me_QueueMindEnumerator.h>

const uint8_t
  IdleSignalValue = HIGH,
  InputSignalPin = 2, // 2 or 3 for ATmega328P
  OutputSignalPin = 12;

const uint16_t
  RecorderCapacity = 40;

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

  pinMode(InputSignalPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(InputSignalPin), OnSignalChange, CHANGE);

  pinMode(OutputSignalPin, INPUT);
}

void OnSignalChange()
{
  DSR.Add(micros(), digitalRead(InputSignalPin));
}

void DelayMcrs(uint32_t Mcrs)
{
  const static uint32_t LargestAccurateMcrsDelay = 16383;
  if (Mcrs <= LargestAccurateMcrsDelay)
    delayMicroseconds(Mcrs);
  else
    delay(Mcrs / 1000);
}

void ReplayRecord(me_DigitalSignalRecorder* DSR)
{
  me_QueueMindEnumerator Cursor(&DSR->Queue);
  pinMode(OutputSignalPin, OUTPUT);
  uint32_t SignalCorrectionDelta = -65;
  uint32_t PauseCorrectionDelta = -30;
  do
  {
    uint32_t Pause = DSR->History[Cursor.Get()].Pause;
    uint32_t Signal = DSR->History[Cursor.Get()].Signal;
    if (Pause > 100)
    {
      Pause += PauseCorrectionDelta;
      digitalWrite(OutputSignalPin, DSR->IdleValue);
      DelayMcrs(Pause);
    }
    if (Signal > 100)
    {
      Signal += SignalCorrectionDelta;
      digitalWrite(OutputSignalPin, !DSR->IdleValue);
      DelayMcrs(Signal);
    }
  } while (Cursor.Move());
  digitalWrite(OutputSignalPin, DSR->IdleValue);
  delay(10);
  pinMode(OutputSignalPin, INPUT);
}

void CopyDSR(me_DigitalSignalRecorder* Source, me_DigitalSignalRecorder* Destination)
{
  Destination->IdleValue = Source->IdleValue;
  if (!Destination->SetCapacity(Source->Queue.GetCapacity()))
  {
    Serial.println("No memory to copy.");
    exit(0);
  }
  me_QueueMindEnumerator Cursor(&Source->Queue);
  uint32_t EmulatedTime = Source->GetFirstEventTime();
  do
  {
    Destination->Add(EmulatedTime, Source->IdleValue);
    EmulatedTime += Source->History[Cursor.Get()].Pause;
    Destination->Add(EmulatedTime, !Source->IdleValue);
    EmulatedTime += Source->History[Cursor.Get()].Signal;
  } while (Cursor.Move());
  Destination->Add(EmulatedTime, Source->IdleValue);
}

void loop()
{
  const uint32_t CutoffDelayToPrintMcrs = 1000000;
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
    interrupts();

    me_DigitalSignalRecorder* DSR_copy = new me_DigitalSignalRecorder(LOW);
    CopyDSR(&DSR, DSR_copy);

    DSR.Clear();

    // DSR_PrintJSON(DSR_copy);
    pinMode(LED_BUILTIN, OUTPUT);
    for (uint8_t i = 0; i < 3; ++i)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(500);
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }
    delay(500);
    ReplayRecord(DSR_copy);
    delete DSR_copy;
  }
  delay(100);
}
