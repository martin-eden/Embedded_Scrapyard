#include "me_StatePrinters_DigitalSignalRecorder.h"

#include <me_QueueMindEnumerator.h>

void DSR_PrintJSON(me_DigitalSignalRecorder* DSR)
{
  if (!DSR->HasEvents() || (DSR->History == NULL))
  {
    Serial.println("null");
    return;
  }

  String s;
  me_QueueMindEnumerator Cursor(&DSR->Queue);

  Serial.println("{");

  Serial.println("  \"Parameters\": {");
  s =
    String("") +
    "    \"Idle signal value\": " + DSR->GetIdleValue() + ",\n"
    "    \"Capacity\": " + DSR->Queue.GetCapacity();
  Serial.println(s);
  Serial.println("  },");

  Serial.println("  \"Durations\": [");
  do
  {
    s =
      String("    {") +
      "\"Pause\": " + DSR->History[Cursor.Get()].Pause + ", " +
      "\"Signal\": " + DSR->History[Cursor.Get()].Signal + "}";
    if (Cursor.Get() != DSR->Queue.GetLastIdx())
      s += ",";
    Serial.println(s);
  } while (Cursor.Next());
  Serial.println("  ],");

  Serial.println("  \"Statistics\": {");
  s =
    String("") +
    "    \"First event\": " + DSR->GetFirstEventTime() + ",\n" +
    "    \"Last event\": " + DSR->GetLastEventTime() + ",\n" +
    "    \"Duration\": " + DSR->GetRecordsDuration() + ",\n" +
    "    \"Num elements\": " + DSR->Queue.GetCount();
  Serial.println(s);

  Serial.println("  }");

  Serial.println("}");
}
