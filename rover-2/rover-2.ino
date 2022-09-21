/*
  Status: stable
  Version: 2.0
  Last mod.: 2022-09-20
*/

#include <SoftwareSerial.h>
#include <me_DcMotor.h>

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
  Command_SetDirection = 'D',
  Command_SetPower = 'P',
  Command_Delimiter = ';',
  Command_PolarCoords_Radius = 'R',
  Command_PolarCoords_Angle = 'A';

SoftwareSerial Connection(ConnectionRX_pin, ConnectionTX_pin);

const t_motor_pins
  LeftMotorPins =
    {
      Forward: LeftMotorForward_pin,
      Backward: LeftMotorBackward_pin
    },
  RightMotorPins =
    {
      Forward: RightMotorForward_pin,
      Backward: RightMotorBackward_pin
    };

DcMotor LeftMotor(LeftMotorPins);
DcMotor RightMotor(RightMotorPins);

void EatDelimiter()
{
  if (Connection.available())
  {
    char c;
    do
    {
      c = Connection.read();
    } while (c != Command_Delimiter);
  }
}

void setup()
{
  Serial.begin(SerialSpeed);
  Serial.println("Hello there!");

  Connection.begin(ConnectionSpeed);
  Connection.setTimeout(ConnectionTimeout_ms);

  Connection.println("Hey there!");
}

void PrintMotorsStatus()
{
  Connection.print("*L");
  Connection.print(AdjustPowerToGauge(LeftMotor.GetScaledPower()));
  Connection.print("*");

  Connection.print("*R");
  Connection.print(AdjustPowerToGauge(RightMotor.GetScaledPower()));
  Connection.print("*");
}

uint16_t AdjustPowerToGauge(int8_t power)
{
  return map(power, -128, 127, 0, 200);
}

void SetDirection(int8_t Direction)
{
  Direction = constrain(Direction, -100, 100);

  /*
    Convert direction to motor coefficients.

    Each coefficient is float in range [-1.0, 1.0].

    No bias:
      Direction = 0: kL = 1.0, kR = 1.0

    45 deg left:
      Direction = -25: kL = cos(), kR = 1.0

    Turn left:
      Direction = -50: kL = 0.0, kR = 1.0

    Skid turn left:
      Direction = -100: kL = -1.0, kR = 1.0
  */

  float x, y;
  x = (float) Direction / 100 * PI;
  y = cos(x);

  float kL, kR;

  if (Direction < 0)
  {
    kL = y;
    kR = 1.0;
  }
  else
  {
    kR = y;
    kL = 1.0;
  }

  LeftMotor.SetScaling(kL);
  RightMotor.SetScaling(kR);
}

void SetPower(int8_t Power)
{
  Power = constrain(Power, -100, 100);

  float z;
  z = (float) Power / 100;
  z *= PI / 2;
  z = sin(z);
  z *= 100;

  Power = z;

  LeftMotor.SetDirectedPower(Power);
  RightMotor.SetDirectedPower(Power);
}

void loop()
{
  static uint32_t LastCommandTime = 0;
  uint32_t CurrentTime = millis();

  if (CurrentTime - LastCommandTime >= PowerOffTimeout_ms)
  {
    LeftMotor.Stop();
    RightMotor.Stop();
  }

  while (Connection.available())
  {
    LastCommandTime = CurrentTime;

    char Command = Connection.read();

    switch(Command)
    {
      case Command_SetDirection:
      {
        /*
          Command format:

            <Command_SetDirection> <[-100, 100]> ";"
        */

        int8_t Direction = Connection.parseInt();
        EatDelimiter();

        SetDirection(Direction);

        PrintMotorsStatus();

        break;
      }

      case Command_SetPower:
      {
        /*
          Command format:

            <Command_SetPower> <[-100, 100]> ";"
        */

        int16_t Power = Connection.parseInt();
        EatDelimiter();

        SetPower(Power);

        PrintMotorsStatus();

        break;
      }

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

        int16_t Power = map(Radius, 1, 100, 0, 100);

        /*
          Map <Angle> to <Direction>: [1, 360] => [-100, 100]:

            Angle | Direction | Notes
            ------+-----------+------
             90   | 100       | Skid turn right
             180  | 0         | Backward
             270  | -100      | Skid turn left
             360  | 0         | Forward
        */
        int16_t Direction;
        if ((Angle >= 1) && (Angle <= 90))
        {
          Direction = map(Angle, 0, 90, 0, 100);
        }
        else if ((Angle >= 90) && (Angle <= 180))
        {
          Direction = map(Angle, 90, 180, -100, 0);
          Power = -Power;
        }
        else if ((Angle >= 180) && (Angle <= 270))
        {
          Direction = map(Angle, 180, 270, 0, 100);
          Power = -Power;
        }
        else if ((Angle >= 270) && (Angle <= 360))
        {
          Direction = map(Angle, 270, 360, -100, 0);
        }

        SetDirection(Direction);
        SetPower(Power);

        PrintMotorsStatus();

        break;
      }

      default:
        Serial.print("Unknown command: \"");
        Serial.print(Command);
        Serial.print("\"");
        Serial.println();
    }
  }

  delay(ProcessingTick_ms);
}
