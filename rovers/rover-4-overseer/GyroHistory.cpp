#include "GyroHistory.h"

#include <ArduinoJson.h>

const uint16_t GyroHistoryCapacity = 5; // 150;
t_GyroHistoryRec GyroHistory[GyroHistoryCapacity];
uint16_t GyroHistoryCursor = 0;

void StoreGyroReadings(MPU6050::t_GyroAccReadings Readings, uint32_t Timestamp_Ms)
{
  t_GyroHistoryRec HistoryRec;

  HistoryRec.Acceleration_Mps.x = Readings.Acceleration_Mps.x;
  HistoryRec.Acceleration_Mps.y = Readings.Acceleration_Mps.y;
  HistoryRec.Acceleration_Mps.z = Readings.Acceleration_Mps.z;

  HistoryRec.Rotation_Dps.x = Readings.Rotation_Dps.x;
  HistoryRec.Rotation_Dps.y = Readings.Rotation_Dps.y;
  HistoryRec.Rotation_Dps.z = Readings.Rotation_Dps.z;

  HistoryRec.Timestamp_Ms = Timestamp_Ms;

  GyroHistory[GyroHistoryCursor] = HistoryRec;

  GyroHistoryCursor = (GyroHistoryCursor + 1) % GyroHistoryCapacity;
}

String SerializeHistoryRec_Json(t_GyroHistoryRec HistoryRec)
{
  String Result = "";

  StaticJsonDocument<192> doc;

  doc["Timestamp_ms"] = HistoryRec.Timestamp_Ms;

  JsonObject Acceleration = doc.createNestedObject("Acceleration_v3_mps");
  Acceleration["X"] = HistoryRec.Acceleration_Mps.x;
  Acceleration["Y"] = HistoryRec.Acceleration_Mps.y;
  Acceleration["Z"] = HistoryRec.Acceleration_Mps.z;

  serializeJson(doc, Result);

  return Result;
}

// Serialize gyro values for Arduino IDE plotter.
String SerializeHistoryRec_Tsv(t_GyroHistoryRec GyroHistoryRec)
{
  const uint8_t NumFractionalDigits = 2;
  String Result = "";

  Result += String(GyroHistoryRec.Acceleration_Mps.x, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Acceleration_Mps.y, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Acceleration_Mps.z, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Rotation_Dps.x, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Rotation_Dps.y, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Rotation_Dps.z, NumFractionalDigits);
  // Result += "\t";
  // Result += GyroHistoryRec.Temperature_C;

  return Result;
}

// Print history of gyro readings.
void PrintGyroHistory()
{
  Serial.println(F("-----------------------------------------------------"));
  uint16_t PrintCursor = GyroHistoryCursor;
  while (true)
  {
    Serial.printf(
      "[%d]\t%s\n",
      PrintCursor,
      SerializeHistoryRec_Tsv(GyroHistory[PrintCursor]).c_str()
    );

    PrintCursor = (PrintCursor + 1) % GyroHistoryCapacity;

    if (PrintCursor == GyroHistoryCursor)
    {
      break;
    }
  }
  Serial.println(F("====================================================="));
}
