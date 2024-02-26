// Gyro history management

/*
  Status: reworking
  Version: 2
  Last mod.: 2023-11-15
*/

#include "GyroHistory.h"

#include <ArduinoJson.h>

const uint16_t GyroHistoryCapacity = 50; // 150;
t_GyroHistoryRec GyroHistory[GyroHistoryCapacity];
uint16_t GyroHistoryCursor = 0;

void StoreGyroReadings(MPU6050::t_GyroAccReadings Readings, uint32_t Timestamp_Ms)
{
  t_GyroHistoryRec HistoryRec;

  HistoryRec.Acceleration_G.x = Readings.Acceleration_G.x;
  HistoryRec.Acceleration_G.y = Readings.Acceleration_G.y;
  HistoryRec.Acceleration_G.z = Readings.Acceleration_G.z;

  HistoryRec.Rotation_Dps.x = Readings.Rotation_Dps.x;
  HistoryRec.Rotation_Dps.y = Readings.Rotation_Dps.y;
  HistoryRec.Rotation_Dps.z = Readings.Rotation_Dps.z;

  HistoryRec.Timestamp_Ms = Timestamp_Ms;

  GyroHistory[GyroHistoryCursor] = HistoryRec;

  GyroHistoryCursor = (GyroHistoryCursor + 1) % GyroHistoryCapacity;
}

/*
  Typical JSON is

    {"Timestamp_ms":17815,"Acceleration_G":{"X":0.01,"Y":0.02,"Z":1.06},"Rotation_dps":{"X":-1.18,"Y":0.39,"Z":-0.85},"Temperature_C":27.75}

  Use it for StaticJsonDocument memory allocation.
*/
String SerializeHistoryRec_Json(t_GyroHistoryRec HistoryRec)
{
  String Result = "";

  StaticJsonDocument<192> doc;

  const uint8_t NumFractionalDigits = 2;

  doc["Timestamp_ms"] = HistoryRec.Timestamp_Ms;

  JsonObject Acceleration = doc.createNestedObject("Acceleration_G");
  Acceleration["X"] = serialized(String(HistoryRec.Acceleration_G.x, NumFractionalDigits));
  Acceleration["Y"] = serialized(String(HistoryRec.Acceleration_G.y, NumFractionalDigits));
  Acceleration["Z"] = serialized(String(HistoryRec.Acceleration_G.z, NumFractionalDigits));

  JsonObject Rotation = doc.createNestedObject("Rotation_Dps");
  Rotation["X"] = serialized(String(HistoryRec.Rotation_Dps.x));
  Rotation["Y"] = serialized(String(HistoryRec.Rotation_Dps.y));
  Rotation["Z"] = serialized(String(HistoryRec.Rotation_Dps.z));

  serializeJson(doc, Result);

  return Result;
}

// Serialize gyro values for Arduino IDE plotter.
String SerializeHistoryRec_Tsv(t_GyroHistoryRec GyroHistoryRec)
{
  const uint8_t NumFractionalDigits = 2;
  String Result = "";

  Result += String(GyroHistoryRec.Acceleration_G.x, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Acceleration_G.y, NumFractionalDigits);
  Result += "\t";
  Result += String(GyroHistoryRec.Acceleration_G.z, NumFractionalDigits);
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

// Serialize history of gyro readings to TSV format
String GetGyroHistory_Tsv()
{
  String Result;

  uint8_t RecordStrMaxSize = 200;
  char RecordStr[RecordStrMaxSize];

  uint16_t PrintCursor = GyroHistoryCursor;
  while (true)
  {
    snprintf(
      RecordStr,
      RecordStrMaxSize,
      "[%d]\t%s\n",
      PrintCursor,
      SerializeHistoryRec_Tsv(GyroHistory[PrintCursor]).c_str()
    );

    Result.concat(RecordStr);

    PrintCursor = (PrintCursor + 1) % GyroHistoryCapacity;

    if (PrintCursor == GyroHistoryCursor)
    {
      break;
    }
  }

  return Result;
}

// Serialize history of gyro readings to JSON format
String GetGyroHistory_Json()
{
  String Result;

  Result += "{\n";

  uint16_t PrintCursor = GyroHistoryCursor;
  do
  {
    Result.concat("  ");
    Result.concat(SerializeHistoryRec_Json(GyroHistory[PrintCursor]));

    PrintCursor = (PrintCursor + 1) % GyroHistoryCapacity;

    if (PrintCursor != GyroHistoryCursor)
      Result.concat(",");
    Result.concat("\n");

  } while (PrintCursor != GyroHistoryCursor);

  Result += "}\n";

  return Result;
}

/*
  2023-11-15
*/
