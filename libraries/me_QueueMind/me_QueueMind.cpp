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

void QueueMind_PrintState(me_QueueMind* Queue)
{
  String msg = "";
  msg +=
    "Idx first, last: " +
    String(Queue->GetFirstIdx()) + ", " +
    String(Queue->GetLastIdx()) + "\n";
  msg +=
    "Num elements: " +
    String(Queue->GetCount()) + " of " +
    String(Queue->GetCapacity()) + "\n";
  msg +=
    "IsFull, IsEmpty: " +
    String(Queue->IsFull()) + ", " +
    String(Queue->IsEmpty()) + "\n";
  Serial.println(msg);
}
