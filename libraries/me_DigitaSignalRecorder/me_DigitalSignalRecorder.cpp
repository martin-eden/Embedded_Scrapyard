#include "me_DigitalSignalRecorder.h"

me_DigitalSignalRecorder::me_DigitalSignalRecorder(uint8_t aIdleValue)
{
  IdleValue = aIdleValue;
  Clear();
}

me_DigitalSignalRecorder::~me_DigitalSignalRecorder()
{
  delete[] History;
}

bool me_DigitalSignalRecorder::SetCapacity(uint16_t aCapacity)
{
  Queue.SetCapacity(aCapacity);
  if (History != NULL)
    delete[] History;
  History = new DurationsRec[aCapacity];
  return (History != NULL);
}

void me_DigitalSignalRecorder::Add(uint32_t CurrentTime, uint8_t Value)
{
  if (History == NULL)
    return;

  // Filter abnormal two same-value calls:
  if ((Value == LastValue) && !Queue.IsEmpty())
    return;

  uint32_t TimePassed = CurrentTime - LastEventTime;
  uint16_t CurIdx;

  // If idle state ended and signal started: calculate pause duration.
  if (Value != IdleValue)
  {
    if (!Queue.Enqueue())
      return;
    CurIdx = Queue.GetLastIdx();
    History[CurIdx].Pause = TimePassed;
    History[CurIdx].Signal = 0;
  }
  // If signal ended and idle state started: calculate signal duration.
  else
  {
    if (!Queue.IsEmpty())
    {
      CurIdx = Queue.GetLastIdx();
      History[CurIdx].Signal = TimePassed;
    }
  }

  LastEventTime = CurrentTime;
  LastValue = Value;
}

bool me_DigitalSignalRecorder::HasEvents()
{
  return !Queue.IsEmpty();
}

uint16_t me_DigitalSignalRecorder::GetNumElements()
{
  if (Queue.IsEmpty())
    return 0;
  if (History[Queue.GetFirstIdx()].Signal == 0)
    return Queue.GetNumElements() - 1;
  return Queue.GetNumElements();
}

bool me_DigitalSignalRecorder::IsFull()
{
  return Queue.IsFull() && (LastValue == IdleValue);
}

void me_DigitalSignalRecorder::Clear()
{
  Queue.Clear();
  LastEventTime = 0;
  LastValue = IdleValue;
}

uint32_t me_DigitalSignalRecorder::GetFirstEventTime()
{
  return HasEvents() ? History[Queue.GetFirstIdx()].Pause : 0;
}

uint32_t me_DigitalSignalRecorder::GetLastEventTime()
{
  return HasEvents() ? LastEventTime : 0;
}

uint32_t me_DigitalSignalRecorder::GetRecordsDuration()
{
  return GetLastEventTime() - GetFirstEventTime();
}

void DSR_PrintJSON(me_DigitalSignalRecorder* DSR)
{
  if (!DSR->HasEvents() || (DSR->History == NULL))
  {
    Serial.println("null");
    return;
  }

  String s;

  Serial.println("{");

  Serial.println("  \"Parameters\": {");
  s =
    String("") +
    "    \"Idle signal value\": " + DSR->IdleValue + ",\n"
    "    \"Capacity\": " + DSR->Queue.GetCapacity();
  Serial.println(s);
  Serial.println("  },");

  Serial.println("  \"Durations\": [");
  uint8_t CurIdx = DSR->Queue.GetFirstIdx();
  while (1)
  {
    s =
      String("    {") +
      "\"Pause\": " + DSR->History[CurIdx].Pause + ", " +
      "\"Signal\": " + DSR->History[CurIdx].Signal + "}";
    if (CurIdx != DSR->Queue.GetLastIdx())
      s += ",";
    Serial.println(s);

    if (CurIdx == DSR->Queue.GetLastIdx())
      break;
    CurIdx = DSR->Queue.GetNextIdx(CurIdx);
  };
  Serial.println("  ],");

  Serial.println("  \"Statistics\": {");
  s =
    String("") +
    "    \"First event\": " + DSR->GetFirstEventTime() + ",\n" +
    "    \"Last event\": " + DSR->GetLastEventTime() + ",\n" +
    "    \"Duration\": " + DSR->GetRecordsDuration() + ",\n" +
    "    \"Num elements\": " + DSR->Queue.GetNumElements();
  Serial.println(s);

  Serial.println("  }");

  Serial.println("}");
}
