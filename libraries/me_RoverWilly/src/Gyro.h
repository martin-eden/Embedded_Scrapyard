#pragma once

#include <Arduino.h>

#include <me_GyroAcc_MPU6050.h>

bool SetupGyro();

void UpdateGyroReadings();

MPU6050::t_GyroAccReadings GetLastGyroReadings();
uint32_t GetLastGyroReadingsTime_Ms();

String SerializeGyroReadings(MPU6050::t_GyroAccReadings GyroReadings, uint32_t Time);
