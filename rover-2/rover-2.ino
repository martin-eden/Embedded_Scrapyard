/*
  Status: stable
  Version: 3.0
  Last mod.: 2022-09-21
*/

#include <SoftwareSerial.h>

#include <me_DcMotor.h>
#include <me_TwoDcMotorsDirector.h>

const uint8_t
  L298N_In1_pin = 5,
  L298N_In2_pin = 6,
  L298N_In3_pin = 9,
  L298N_In4_pin = 10,
  HC05_TXD_pin = 13,
  HC05_RXD_pin = 12,

  LeftMotorForward_pin = L298N_In4_pin,
  LeftMotorBackward_pin = L298N_In3_pin,

  RightMotorForward_pin = L298N_In1_pin,
  RightMotorBackward_pin = L298N_In2_pin,

  ConnectionRX_pin = HC05_TXD_pin,
  ConnectionTX_pin = HC05_RXD_pin;

const uint32_t
  SerialSpeed = 57600,
  ConnectionSpeed = 57600,
  ConnectionTimeout_ms = 50,
  ProcessingTick_ms = 100,
  PowerOffTimeout_ms = 500;

const char
  Command_PolarCoords_Radius = 'R',
  Command_PolarCoords_Angle = 'A';

const TMotorPins
  LeftMotorPins =
    {
      ForwardPin: LeftMotorForward_pin,
      BackwardPin: LeftMotorBackward_pin
    },
  RightMotorPins =
    {
      ForwardPin: RightMotorForward_pin,
      BackwardPin: RightMotorBackward_pin
    };

SoftwareSerial Connection(ConnectionRX_pin, ConnectionTX_pin);

DcMotor LeftMotor(LeftMotorPins);
DcMotor RightMotor(RightMotorPins);

TwoDcMotorsDirector MotorsDirector(&LeftMotor, &RightMotor);

void setup()
{
  Serial.begin(SerialSpeed);

  Serial.println("Serial: Hello there!");

  Connection.begin(ConnectionSpeed);
  Connection.setTimeout(ConnectionTimeout_ms);

  Connection.println("Bluetooth: Hey there!");
}

void PrintMotorsStatus()
{
  Connection.print("*L");
  Connection.print(AdjustToPowerGauge(LeftMotor));
  Connection.print("*");

  Connection.print("*R");
  Connection.print(AdjustToPowerGauge(RightMotor));
  Connection.print("*");
}

int16_t AdjustToPowerGauge(DcMotor Motor)
{
  int16_t Result;

  Result = Motor.GetPower();
  Result = map(Result, 0, 255, 0, 100);
  if (Motor.GetIsBackward())
    Result = -Result;

  return Result;
}

void loop()
{
  static uint32_t LastCommandTime = 0;
  uint32_t CurrentTime = millis();

  if (CurrentTime - LastCommandTime >= PowerOffTimeout_ms)
  {
    MotorsDirector.SetPower(0);
  }

  while (Connection.available())
  {
    LastCommandTime = CurrentTime;

    char Command = Connection.read();

    switch(Command)
    {
      case Command_PolarCoords_Radius:
      {
        /*
          Command format:

            <Command_PolarCoords_Radius> <[1, 100]> <Command_PolarCoords_Angle> <[1, 360]>
        */
        uint8_t Radius = Connection.parseInt();
        Radius = constrain(Radius, 1, 100);

        char NextToken = Connection.read();

        if (NextToken != Command_PolarCoords_Angle)
          break;

        uint16_t Angle = Connection.parseInt();
        Angle = constrain(Angle, 1, 360);

        int16_t Direction = Angle % 360;
        int16_t Power = map(Radius, 1, 100, 0, 255);

        MotorsDirector.SetDirection(Direction);
        MotorsDirector.SetPower(Power);

        PrintMotorsStatus();

        break;
      }

      default:
      {
        Serial.print("?(");
        Serial.print(Command);
        Serial.print(")");
        Serial.println();

        break;
      }
    }
  }

  delay(ProcessingTick_ms);
}