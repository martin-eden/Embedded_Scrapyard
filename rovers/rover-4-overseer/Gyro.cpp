#include "Gyro.h"

#include <ArduinoJson.h>

#include "Math.h"

MPU6050::t_GyroAccReadings LastGyroReadings;
uint32_t LastGyroReadingsTime_Ms = 0;

MPU6050::t_GyroAcc GyroAcc;

bool SetupGyro()
{
  bool IsGyroInitialized = false;

  const uint8_t MaxNumTries = 10;
  const uint8_t TriesInterval_Ms = 100;

  Serial.print("Initializing gyro..");
  for (uint8_t NumTries = 0; NumTries < MaxNumTries; ++NumTries)
  {
    IsGyroInitialized = GyroAcc.Initialize();

    if (IsGyroInitialized)
    {
      break;
    }

    delay(TriesInterval_Ms);

    Serial.print(".");
  }

  Serial.print(" ");

  if (!IsGyroInitialized)
  {
    Serial.println("nah.");
  }
  else
  {
    Serial.println("yep.");
  }

  return IsGyroInitialized;
}

MPU6050::t_GyroAccReadings GetGyroReadings();

void UpdateGyroReadings()
{
  MPU6050::t_GyroAccReadings GyroReadings = GetGyroReadings();

  LastGyroReadings = GyroReadings;
  LastGyroReadingsTime_Ms = millis();
}

MPU6050::t_GyroAccReadings GetLastGyroReadings()
{
  return LastGyroReadings;
}

uint32_t GetLastGyroReadingsTime_Ms()
{
  return LastGyroReadingsTime_Ms;
}

/*
  Get gyro readings (acceleration, rotation, temperature) and process
  them before returning.

  Values are rounded to some granularity value.

  This is done to reduce minor fluctuations in values (jitter).

  For cases when value is between two granulated points, jitter still
  persists for and just amplified to granularity value. (Fluctuations
  (1.49, 1.51, 1.49, ...) are rounded to (1.0, 2.0, 1.0, ...).)

  Jitter can be reduced by capacitive filtering before granulating but
  current implementation is good enough for now.
*/
MPU6050::t_GyroAccReadings GetGyroReadings()
{
  // Granularity for float values.
  const float
    GranularityUnit_Mps = 0.25,
    GranularityUnit_Dps = 0.25,
    GranularityUnit_C = 0.25;

  MPU6050::t_GyroAccReadings Result;

  Result = GyroAcc.GetReadings();

  Result.Acceleration_Mps.x = RoundToUnit(Result.Acceleration_Mps.x, GranularityUnit_Mps);
  Result.Acceleration_Mps.y = RoundToUnit(Result.Acceleration_Mps.y, GranularityUnit_Mps);
  Result.Acceleration_Mps.z = RoundToUnit(Result.Acceleration_Mps.z, GranularityUnit_Mps);

  Result.Rotation_Dps.x = RoundToUnit(Result.Rotation_Dps.x, GranularityUnit_Dps);
  Result.Rotation_Dps.y = RoundToUnit(Result.Rotation_Dps.y, GranularityUnit_Dps);
  Result.Rotation_Dps.z = RoundToUnit(Result.Rotation_Dps.z, GranularityUnit_Dps);

  Result.Temperature_C = RoundToUnit(Result.Temperature_C, GranularityUnit_C);

  return Result;
}

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings, uint32_t Time)
{
  String Result = "";
  StaticJsonDocument<192> doc;

  doc["Timestamp_ms"] = Time;

  JsonObject Acceleration_V3 = doc.createNestedObject("Acceleration_v3_mps");
  Acceleration_V3["X"] = GyroReadings.Acceleration_Mps.x;
  Acceleration_V3["Y"] = GyroReadings.Acceleration_Mps.y;
  Acceleration_V3["Z"] = GyroReadings.Acceleration_Mps.z;

  JsonObject Rotation_V3 = doc.createNestedObject("Rotation_v3_dps");
  Rotation_V3["X"] = GyroReadings.Rotation_Dps.x;
  Rotation_V3["Y"] = GyroReadings.Rotation_Dps.y;
  Rotation_V3["Z"] = GyroReadings.Rotation_Dps.z;
  doc["Temperature_C"] = GyroReadings.Temperature_C;

  serializeJson(doc, Result);

  return Result;
}
