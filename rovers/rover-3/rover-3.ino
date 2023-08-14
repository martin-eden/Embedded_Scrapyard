// Firmware for my Rover v3.

/*
  Status: works
  Version: 4
  Last mod.: 2023-08-13
*/

/*
  Board: Arduino Nano
  Processor: ATMega16
*/

#include <SoftwareSerial.h>

#include <me_DcMotor.h>
#include <me_TwoDcMotorsDirector.h>

const uint8_t
  L298N_In1_pin = 5,
  L298N_In2_pin = 6,
  L298N_In3_pin = 9,
  L298N_In4_pin = 10,
  HC05_TXD_pin = 12,
  HC05_RXD_pin = 11,

  LeftMotorForward_pin = L298N_In3_pin,
  LeftMotorBackward_pin = L298N_In4_pin,

  RightMotorForward_pin = L298N_In2_pin,
  RightMotorBackward_pin = L298N_In1_pin,

  ConnectionRX_pin = HC05_TXD_pin,
  ConnectionTX_pin = HC05_RXD_pin;

const uint32_t
  SerialSpeed = 57600,
  ConnectionSpeed = 57600,
  ConnectionTimeout_ms = 20,
  ProcessingTick_ms = 25,
  PowerOffTimeout_ms = 350;

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

void SendStatus()
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

  Result = Motor.GetActualSpeed();
  Result = map(Result, -100, 100, 0, 200);

  return Result;
}

static uint32_t LastCommandTime = 0;

void PrintDebugInfo()
{
  Serial.print("Direction: ");
  Serial.print(MotorsDirector.GetDirection());
  Serial.println();

  Serial.print("Power: ");
  Serial.print(MotorsDirector.GetPower());
  Serial.println();
}

void loop()
{
  uint32_t CurrentTime = millis();

  SendStatus();

  // PrintDebugInfo();

  uint32_t TimeSinceLastCommand_ms = CurrentTime - LastCommandTime;

  if (TimeSinceLastCommand_ms > PowerOffTimeout_ms)
    MotorsDirector.SetPower(0);

  while (Connection.available())
  {
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

        int16_t Direction = 359 - (Angle % 360);
        uint8_t Power = map(Radius, 1, 100, 0, 255);

        MotorsDirector.SetDirection(Direction);
        MotorsDirector.SetPower(Power);

        LastCommandTime = CurrentTime;

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

  MotorsDirector.Update();

  delay(ProcessingTick_ms);
}

/*
  2023-06-04
  2023-08-13
*/
