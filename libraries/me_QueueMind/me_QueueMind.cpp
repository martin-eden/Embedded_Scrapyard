#include <me_QueueMind.h>

const uint16_t InvalidIdx = 0xFFFF;

me_QueueMind::me_QueueMind(uint16_t aCapacity)
{
  SetCapacity(aCapacity);
}

void me_QueueMind::Clear()
{
  FirstIdx = InvalidIdx;
  LastIdx = InvalidIdx;
}

uint16_t me_QueueMind::GetFirstIdx()
{
  return FirstIdx;
}

uint16_t me_QueueMind::GetLastIdx()
{
  return LastIdx;
}

uint16_t me_QueueMind::GetNumElements()
{
  if ((FirstIdx == InvalidIdx) && (LastIdx == InvalidIdx))
    return 0;
  if (FirstIdx <= LastIdx)
    return (LastIdx - FirstIdx + 1);
  else
    return (LastIdx + Capacity - FirstIdx + 1);
}

uint16_t me_QueueMind::GetCapacity()
{
  return Capacity;
}

void me_QueueMind::SetCapacity(uint16_t aCapacity)
{
  Capacity = aCapacity;
  Clear();
}

uint16_t me_QueueMind::GetNextIdx(uint16_t aIdx)
{
  return (aIdx + 1) % Capacity;
}

bool me_QueueMind::IsFull()
{
  return (GetNumElements() == Capacity);
}

bool me_QueueMind::IsEmpty()
{
  return (GetNumElements() == 0);
}

bool me_QueueMind::Enqueue()
{
  if (IsFull())
    return false;

  if (IsEmpty())
  {
    FirstIdx = 0;
    LastIdx = 0;
  }
  else
    LastIdx = GetNextIdx(LastIdx);

  return true;
}

bool me_QueueMind::Dequeue()
{
  if (IsEmpty())
    return false;

  if (GetNumElements() == 1)
    Clear();
  else
    FirstIdx = GetNextIdx(FirstIdx);

  return true;
}

void QueueMind_PrintState(me_QueueMind* Queue)
{
  String msg = "";
  msg +=
    "Idx first: " +
    String(Queue->GetFirstIdx()) + "\n";
  msg +=
    "Num elements: " +
    String(Queue->GetNumElements()) + " of " +
    String(Queue->GetCapacity()) + "\n";
  msg +=
    "IsFull, IsEmpty: " +
    String(Queue->IsFull()) + ", " +
    String(Queue->IsEmpty()) + "\n";
  Serial.println(msg);
}
