// Interface for gyroscope/accelerometer MPU6050

/*
  Status: works
  Version: 4
  Last mod.: 2023-11-15
*/

#include "me_GyroAcc_MPU6050.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

#include <me_Math.h>
#include <me_Math_Physics.h>

using namespace MPU6050;

bool t_GyroAcc::Initialize()
{
  if (!GyroAcc.begin())
  {
    return false;
  }

  GyroAcc.setAccelerometerRange(MPU6050_RANGE_2_G);
  GyroAcc.setGyroRange(MPU6050_RANGE_500_DEG);

  GyroAcc.setFilterBandwidth(MPU6050_BAND_260_HZ);

  return true;
}

t_GyroAccReadings t_GyroAcc::GetReadings()
{
  sensors_event_t Acceleration, Rotation, Temperature;
  t_GyroAccReadings Result;

  GyroAcc.getEvent(&Acceleration, &Rotation, &Temperature);

  // Finally, convenient Gs!
  Result.Acceleration_G.x = MpsToG(Acceleration.acceleration.x);
  Result.Acceleration_G.y = MpsToG(Acceleration.acceleration.y);
  Result.Acceleration_G.z = MpsToG(Acceleration.acceleration.z);

  /*
    Actually IMU6050 gives values in degrees per second.
    It's Adafruit library comverts it to radians.
  */
  Result.Rotation_Dps.x = RadToDeg(Rotation.gyro.x);
  Result.Rotation_Dps.y = RadToDeg(Rotation.gyro.y);
  Result.Rotation_Dps.z = RadToDeg(Rotation.gyro.z);

  Result.Temperature_C = Temperature.temperature;

  return Result;
}

/*
  2023-09-22
  2023-10-12
  2023-11-03
  2023-11-15
*/
