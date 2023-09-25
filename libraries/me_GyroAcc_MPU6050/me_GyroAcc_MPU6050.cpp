// Representation model for gyroscope/accelerometer MPU6050

/*
  Version: 1
  Last mod.: 2023-09-22
*/

/*
*/

#include "me_GyroAcc_MPU6050.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

using namespace MPU6050;

t_GyroAcc::t_GyroAcc()
{
}

void t_GyroAcc::Configure()
{
 if (!GyroAcc.begin())
  {
    Serial.println("Failed to open MPU6050 gyro/acc.");
    return;
  }

  GyroAcc.setAccelerometerRange(MPU6050_RANGE_8_G);
  GyroAcc.setGyroRange(MPU6050_RANGE_500_DEG);
  GyroAcc.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Gyro/acc MPU6050 successfully configured.");
}

t_GyroAccReadings t_GyroAcc::GetReadings()
{
  sensors_event_t a, g, temp;
  GyroAcc.getEvent(&a, &g, &temp);

  t_GyroAccReadings Result;

  Result.Acceleration.x = a.acceleration.x;
  Result.Acceleration.y = a.acceleration.y;
  Result.Acceleration.z = a.acceleration.z;

  Result.Rotation.x = g.gyro.x;
  Result.Rotation.y = g.gyro.y;
  Result.Rotation.z = g.gyro.z;

  return Result;
}
