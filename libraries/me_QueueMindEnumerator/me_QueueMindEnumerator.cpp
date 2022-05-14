#include <me_QueueMindEnumerator.h>

me_QueueMindEnumerator::me_QueueMindEnumerator(me_QueueSubconscious* aQueue)
{
  Queue = aQueue;
  Reset();
}

void me_QueueMindEnumerator::Reset()
{
  Idx = Queue->GetFirstIdx();
}

uint16_t me_QueueMindEnumerator::Get()
{
  return Idx;
}

bool me_QueueMindEnumerator::Next()
{
  if (Queue->IsEmpty())
    return false;
  if (Idx == Queue->GetLastIdx())
    return false;

  Idx = Queue->GetNextIdx(Idx);

  return true;
}
