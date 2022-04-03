#include <me_QueueMind.h>

bool me_QueueMind::Enqueue()
{
  if (IsFull())
    return false;

  if (IsEmpty())
  {
    FirstIdx = 0;
    LastIdx = 0;
    _IsEmpty = false;
  }
  else
    LastIdx = GetNextIdx(LastIdx);

  return true;
}

bool me_QueueMind::Dequeue()
{
  if (IsEmpty())
    return false;

  if (GetCount() == 1)
    MakeEmpty();
  else
    FirstIdx = GetNextIdx(FirstIdx);

  return true;
}
