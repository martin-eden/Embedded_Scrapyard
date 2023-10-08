// Firmware for my Rover v4.

/*
  Status: writing
  Version: 1
  Last mod.: 2023-10-07
*/

/*
  Board: WeMos D1 ESP WROOM WiFi UNO
  Processor: ESP 8266
  Motor board: Deek
*/

#include <ESP8266WiFi.h>

#include <me_DeekMotor.h>
// #include <me_TwoDcMotorsDirector.h>


const uint8_t
  Deek_DirA_Pin = D12,
  Deek_PwmA_Pin = D3,
  Deek_BrakeA_Pin = D8,

  Deek_DirB_Pin = D13,
  Deek_PwmB_Pin = D11,
  Deek_BrakeB_Pin = D9;

const uint32_t
  SerialSpeed = 115200;

const TDeekMotorPins
  MotorPins =
    {
      Direction_Pin: Deek_DirA_Pin,
      Pwm_Pin: Deek_PwmA_Pin,
      Brake_Pin: Deek_BrakeA_Pin
    };

DeekMotor Motor(MotorPins);

void setup()
{
  Serial.begin(SerialSpeed);
  Serial.println();
  Serial.println("---------------------");
  Serial.println("Rover-4: Hello there!");
  Serial.println("---------------------");
}

void loop()
{
  Serial.println("-------------------");
  Serial.println("Rover-4: Loop here!");
  Serial.println("-------------------");

  for (int16_t angle_deg = 0; angle_deg < 360; ++angle_deg)
  {
    int8_t DesiredSpeed = 0;

    float MagnifiedSine = sin(DEG_TO_RAD * angle_deg) * 100;

    DesiredSpeed = MagnifiedSine;

    Motor.SetDesiredSpeed(DesiredSpeed);

    Serial.println(DesiredSpeed);

    Motor.Update();
    delay(50);
    Motor.Update();
    delay(50);
    Motor.Update();
    delay(50);
    Motor.Update();
    delay(50);
  }
}

/*
  2023-10-07
*/
