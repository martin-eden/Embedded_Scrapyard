#include "me_StatePrinters_QueueMind.h"

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
