// Representation model for gyroscope/accelerometer MPU6050

/*
  Status: works
  Version: 1
  Last mod.: 2023-10-12
*/

#include "me_GyroAcc_MPU6050.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

using namespace MPU6050;

bool t_GyroAcc::Initialize()
{
  if (!GyroAcc.begin())
  {
    return false;
  }

  GyroAcc.setAccelerometerRange(MPU6050_RANGE_4_G);
  GyroAcc.setGyroRange(MPU6050_RANGE_500_DEG);
  GyroAcc.setFilterBandwidth(MPU6050_BAND_94_HZ);

  return true;
}

t_GyroAccReadings t_GyroAcc::GetReadings()
{
  sensors_event_t Acceleration, Rotation, Temperature;
  t_GyroAccReadings Result;

  GyroAcc.getEvent(&Acceleration, &Rotation, &Temperature);

  Result.Acceleration_Mps.x = Acceleration.acceleration.x;
  Result.Acceleration_Mps.y = Acceleration.acceleration.y;
  Result.Acceleration_Mps.z = Acceleration.acceleration.z;

  Result.Rotation_Dps.x = Rotation.gyro.x;
  Result.Rotation_Dps.y = Rotation.gyro.y;
  Result.Rotation_Dps.z = Rotation.gyro.z;

  Result.Temperature_C = Temperature.temperature;

  return Result;
}

/*
  2023-09-22
  2023-10-12
*/
