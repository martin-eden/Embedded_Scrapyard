#include "Gyro.h"

#include <ArduinoJson.h>

#include <me_Math.h>

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
    Granularity_Mps = 1.0 / 100,
    Granularity_Dps = 1.0 / 360,
    Granularity_C = 1.0 / 4;

  MPU6050::t_GyroAccReadings Result;

  Result = GyroAcc.GetReadings();

  Result.Acceleration_Mps.x = RoundToUnit(Result.Acceleration_Mps.x, Granularity_Mps);
  Result.Acceleration_Mps.y = RoundToUnit(Result.Acceleration_Mps.y, Granularity_Mps);
  Result.Acceleration_Mps.z = RoundToUnit(Result.Acceleration_Mps.z, Granularity_Mps);

  Result.Rotation_Dps.x = RoundToUnit(Result.Rotation_Dps.x, Granularity_Dps);
  Result.Rotation_Dps.y = RoundToUnit(Result.Rotation_Dps.y, Granularity_Dps);
  Result.Rotation_Dps.z = RoundToUnit(Result.Rotation_Dps.z, Granularity_Dps);

  Result.Temperature_C = RoundToUnit(Result.Temperature_C, Granularity_C);

  return Result;
}

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings, uint32_t Time)
{
  const uint8_t NumFractionalDigits = 2;

  String Result = "";
  StaticJsonDocument<300> doc;

  doc["Timestamp_ms"] = Time;

  /*
    I hate C++ templates.

    Code snippet

      Acceleration["X"] = serialized(String(Value, NumFractionalDigits));

    works.

    But if I want to wrap those "serialized(String" noise in function,
    it wont compile:

      SerializedValue SerializeFloat(float Value, uint8_t NumFractionalDigits = 2)
      {
        return serialized(String(Value, NumFractionalDigits));
      }

      Acceleration["X"] = SerializeFloat(Value);

    Compiler will say something about "error: deduced class type".

    So I have to keep this low-entropy code.
  */

  JsonObject Acceleration = doc.createNestedObject("Acceleration_mps");
  Acceleration["X"] = serialized(String(GyroReadings.Acceleration_Mps.x, NumFractionalDigits));
  Acceleration["Y"] = serialized(String(GyroReadings.Acceleration_Mps.y, NumFractionalDigits));
  Acceleration["Z"] = serialized(String(GyroReadings.Acceleration_Mps.z, NumFractionalDigits));

  JsonObject Rotation = doc.createNestedObject("Rotation_dps");
  Rotation["X"] = serialized(String(GyroReadings.Rotation_Dps.x, NumFractionalDigits));
  Rotation["Y"] = serialized(String(GyroReadings.Rotation_Dps.y, NumFractionalDigits));
  Rotation["Z"] = serialized(String(GyroReadings.Rotation_Dps.z, NumFractionalDigits));

  doc["Temperature_C"] = serialized(String(GyroReadings.Temperature_C, NumFractionalDigits));

  serializeJson(doc, Result);

  return Result;
}
