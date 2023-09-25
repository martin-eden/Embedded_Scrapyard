// Representation model for gyroscope/accelerometer MPU6050

/*
  Version: 1
  Last mod.: 2023-09-22
*/

/*
*/

#pragma once

#include <Arduino.h>
#include <Adafruit_MPU6050.h>

namespace MPU6050
{
  struct t_Acceleration
  {
    float x;
    float y;
    float z;
  };

  struct t_Rotation
  {
    float x;
    float y;
    float z;
  };

  struct t_GyroAccReadings
  {
    t_Acceleration Acceleration;
    t_Rotation Rotation;
  };

  class t_GyroAcc
  {
    public:
      t_GyroAcc();
      void Configure();
      t_GyroAccReadings GetReadings();

    private:
      Adafruit_MPU6050 GyroAcc;
  };
}
