// Interface for gyroscope/accelerometer MPU6050

/*
  Version: 1
  Last mod.: 2023-10-25
*/

/*
  Implementation is based on Adafruit library. I just want to have
  consistent structure naming over my code.
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
    t_Acceleration Acceleration_Mps;
    t_Rotation Rotation_Dps;
    float Temperature_C;
  };

  class t_GyroAcc
  {
    public:
      bool Initialize();

      t_GyroAccReadings GetReadings();

    private:
      Adafruit_MPU6050 GyroAcc;
  };
}

/*
  2023-09-22
  2023-10-12
*/
