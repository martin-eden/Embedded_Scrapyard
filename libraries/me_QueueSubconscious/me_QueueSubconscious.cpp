#include <me_QueueSubconscious.h>

me_QueueSubconscious::me_QueueSubconscious(uint16_t aCapacity)
{
  MakeEmpty();
  SetCapacity(aCapacity);
}

bool me_QueueSubconscious::IsEmpty()
{
  return _IsEmpty;
}

void me_QueueSubconscious::MakeEmpty()
{
  FirstIdx = 0;
  LastIdx = 0;
  _IsEmpty = true;
}

uint16_t me_QueueSubconscious::GetCapacity()
{
  return Capacity;
}

void me_QueueSubconscious::SetCapacity(uint16_t aCapacity)
{
  Capacity = aCapacity;
  MakeEmpty();
}

uint16_t me_QueueSubconscious::GetFirstIdx()
{
  return FirstIdx;
}

uint16_t me_QueueSubconscious::GetLastIdx()
{
  return LastIdx;
}

uint16_t me_QueueSubconscious::GetCount()
{
  if (IsEmpty())
    return 0;

  // This formula depends on GetNextIdx() behaviour.
  // The general way to determine length is call
  // GetNextIdx() from <FirstIdx> until <LastIdx>.
  // But this has complexity O(N).
  if (FirstIdx <= LastIdx)
    return (LastIdx - FirstIdx + 1);
  else
    return (LastIdx + Capacity - FirstIdx + 1);
}

bool me_QueueSubconscious::IsFull()
{
  return (GetCount() == Capacity);
}

uint16_t me_QueueSubconscious::GetNextIdx(uint16_t aIdx)
{
  return (aIdx + 1) % Capacity;
}
