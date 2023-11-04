#pragma once

#include <Arduino.h>

#include "Gyro.h"

/*
  For gyro readings history we need timestamp and can store subset of
  gyro each gyro status to reduce memory consumption.
*/
struct t_GyroHistoryRec
{
  uint32_t Timestamp_Ms;
  struct
  {
    float x;
    float y;
    float z;
  } Acceleration_G;
  struct
  {
    float x;
    float y;
    float z;
  } Rotation_Dps;
};

void StoreGyroReadings(MPU6050::t_GyroAccReadings Readings, uint32_t Timestamp_Ms);
String SerializeHistoryRec_Json(t_GyroHistoryRec HistoryRec);
String SerializeHistoryRec_Tsv(t_GyroHistoryRec GyroHistoryRec);
void PrintGyroHistory();
