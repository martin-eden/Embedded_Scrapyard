// Working with one DC motor via Deek motor shield.

/*
  Version: 1
  Last mod.: 2023-10-07
*/

#include "me_DeekMotor.h"

DeekMotor::DeekMotor(TDeekMotorPins aMotorPins)
{
  MotorPins = aMotorPins;

  pinMode(MotorPins.Direction_Pin, OUTPUT);
  pinMode(MotorPins.Pwm_Pin, OUTPUT);
  pinMode(MotorPins.Brake_Pin, OUTPUT);

  digitalWrite(MotorPins.Direction_Pin, LOW);
  digitalWrite(MotorPins.Pwm_Pin, LOW);
  digitalWrite(MotorPins.Brake_Pin, LOW);

  Speed = 0;

  SetSpeed(Speed);
}

void DeekMotor::SetSpeed(int8_t aSpeed)
{
  Speed = aSpeed;

  Speed = constrain(Speed, -100, 100);

  Actualize();
}

int8_t DeekMotor::GetSpeed()
{
  return Speed;
}

// -= private =-

void DeekMotor::Actualize()
/*
  Set motor speed to <ActualSpeed>.

  Depending on sign of speed, we set direcion pin.
  Depending on magniture of speed, we set PWM pin.
*/
{
  uint8_t Direction;
  uint8_t Pwm;

  Direction = (Speed < 0) ? HIGH : LOW;

  Pwm = map(abs(Speed), 0, 100, 0, 255);

  digitalWrite(MotorPins.Direction_Pin, Direction);
  analogWrite(MotorPins.Pwm_Pin, Pwm);
}

/*
  2023-08-13 - inherited
  2023-10-07
*/
