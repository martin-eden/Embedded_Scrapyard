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
    if (Queue.IsEmpty())
    {
      FirstEventTime = CurrentTime;
      TimePassed = 0;
    }
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
  return (GetCount() != 0);
}

uint16_t me_DigitalSignalRecorder::GetCount()
{
  if (Queue.IsEmpty())
    return 0;
  if (History[Queue.GetLastIdx()].Signal == 0)
    return Queue.GetCount() - 1;
  return Queue.GetCount();
}

bool me_DigitalSignalRecorder::IsFull()
{
  return Queue.IsFull() && (LastValue == IdleValue);
}

void me_DigitalSignalRecorder::Clear()
{
  Queue.MakeEmpty();
  FirstEventTime = 0;
  LastEventTime = 0;
  LastValue = IdleValue;
}

uint8_t me_DigitalSignalRecorder::GetIdleValue()
{
  return IdleValue;
}

uint32_t me_DigitalSignalRecorder::GetFirstEventTime()
{
  return FirstEventTime;
}

uint32_t me_DigitalSignalRecorder::GetLastEventTime()
{
  return LastEventTime;
}

uint32_t me_DigitalSignalRecorder::GetRecordsDuration()
{
  return GetLastEventTime() - GetFirstEventTime();
}
