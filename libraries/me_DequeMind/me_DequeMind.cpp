#include <me_DequeMind.h>

const uint16_t InvalidIdx = 0xFFFF;

me_DequeMind::me_DequeMind(uint16_t aCapacity)
{
  SetCapacity(aCapacity);
}

void me_DequeMind::Clear()
{
  FirstIdx = InvalidIdx;
  LastIdx = InvalidIdx;
}

uint16_t me_DequeMind::GetFirstIdx()
{
  return FirstIdx;
}

uint16_t me_DequeMind::GetLastIdx()
{
  return LastIdx;
}

uint16_t me_DequeMind::GetNumElements()
{
  if ((FirstIdx == InvalidIdx) && (LastIdx == InvalidIdx))
    return 0;
  if (FirstIdx <= LastIdx)
    return (LastIdx - FirstIdx + 1);
  else
    return (LastIdx + Capacity - FirstIdx + 1);
}

uint16_t me_DequeMind::GetCapacity()
{
  return Capacity;
}

void me_DequeMind::SetCapacity(uint16_t aCapacity)
{
  Capacity = aCapacity;
  Clear();
}

uint16_t me_DequeMind::GetNextIdx(uint16_t aIdx)
{
  return (aIdx + 1) % Capacity;
}

uint16_t me_DequeMind::GetPrevIdx(uint16_t aIdx)
{
  return (aIdx + Capacity - 1) % Capacity;
}

bool me_DequeMind::IsFull()
{
  return (GetNumElements() == Capacity);
}

bool me_DequeMind::IsEmpty()
{
  return (GetNumElements() == 0);
}

bool me_DequeMind::AddBefore()
{
  if (IsFull())
    return false;

  if (IsEmpty())
  {
    FirstIdx = 0;
    LastIdx = 0;
  }
  else
    FirstIdx = GetPrevIdx(FirstIdx);

  return true;
}

bool me_DequeMind::AddAfter()
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

bool me_DequeMind::RemoveFirst()
{
  if (IsEmpty())
    return false;

  if (GetNumElements() == 1)
    Clear();
  else
    FirstIdx = GetNextIdx(FirstIdx);

  return true;
}

bool me_DequeMind::RemoveLast()
{
  if (IsEmpty())
    return false;

  if (GetNumElements() == 1)
    Clear();
  else
    LastIdx = GetPrevIdx(LastIdx);

  return true;
}

void DequeMind_PrintState(me_DequeMind* Queue)
{
  String msg = "";
  msg +=
    "Idx first, last: " +
    String(Queue->GetFirstIdx()) + ", " +
    String(Queue->GetLastIdx()) + "\n";
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
