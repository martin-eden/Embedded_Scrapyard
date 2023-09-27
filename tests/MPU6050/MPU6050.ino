// MPU6050 basic test.

/*
  Status: works
  Version: 1
  Last mod.: 2023-09-25
*/

#include <me_GyroAcc_MPU6050.h>

MPU6050::t_GyroAcc GyroAcc;

uint16_t MeasurementsPeriod_ms = 1500;

void setup()
{
  Serial.begin(115200);
  Serial.println("Gyro/acc MPU6050 test.");
  GyroAcc.Initialize();
}

void loop()
{
  MPU6050::t_GyroAccReadings GyroAccReadings;
  GyroAccReadings = GyroAcc.GetReadings();

  Serial.print("Acceleration");
  Serial.print("(");
  Serial.print(GyroAccReadings.Acceleration.x);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Acceleration.y);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Acceleration.z);
  Serial.print(")");
  Serial.print(" ");
  Serial.print("Rotation");
  Serial.print("(");
  Serial.print(GyroAccReadings.Rotation.x);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Rotation.y);
  Serial.print(" ");
  Serial.print(GyroAccReadings.Rotation.z);
  Serial.print(")");
  Serial.print(" ");
  Serial.print("Temperature");
  Serial.print("(");
  Serial.print(GyroAccReadings.Temperature);
  Serial.print(")");
  Serial.print("\n");

  delay(MeasurementsPeriod_ms);
}
