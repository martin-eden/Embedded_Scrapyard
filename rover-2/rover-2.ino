/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-09-20
*/

#include <SoftwareSerial.h>

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
  ProcessingTick_ms = 250;

const char
  CommandOn = 'O',
  CommandOff = 'F',
  CommandSetDirection = 'D',
  CommandSetPower = 'P',
  CommandDelimiter = ';';

SoftwareSerial Connection(ConnectionRX_pin, ConnectionTX_pin);

struct t_motor_pins
{
  int Forward;
  int Backward;
};

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

class DCMotor
{
  public:
    DCMotor(t_motor_pins motorPins);

    void Stop();

    void SetDirectedPower(int8_t DirectedPower);
    void SetScaling(float scale);
    float GetScaling();
    int8_t GetScaledPower();

  private:
    t_motor_pins pins;
    int8_t Power = 0;
    bool IsBackward();
    float PowerScale;

    void Actualize();
    void SetPower();
    void SetDirection();
    uint8_t GetPwmPower();
};

DCMotor LeftMotor(LeftMotorPins);
DCMotor RightMotor(RightMotorPins);

DCMotor::DCMotor(t_motor_pins motorPins)
{
  this->pins = motorPins;

  pinMode(pins.Forward, OUTPUT);
  pinMode(pins.Backward, OUTPUT);

  Stop();
  SetScaling(1.0);
}

void DCMotor::Stop()
{
  Power = 0;
  Actualize();
}

void DCMotor::SetScaling(float scale)
{
  PowerScale = constrain(scale, -1.0, 1.0);

  Actualize();
}

void DCMotor::SetDirectedPower(int8_t DirectedPower)
{
  DirectedPower = constrain(DirectedPower, -100, 100);
  Power = map(DirectedPower, -100, 100, -128, 127);

  Actualize();
}

void DCMotor::Actualize()
{
  if (Power == 0)
  {
    digitalWrite(pins.Forward, LOW);
    digitalWrite(pins.Backward, LOW);
  }
  else if (IsBackward())
  {
    digitalWrite(pins.Forward, LOW);
    analogWrite(pins.Backward, GetPwmPower());
  }
  else
  {
    digitalWrite(pins.Backward, LOW);
    analogWrite(pins.Forward, GetPwmPower());
  }
}

bool DCMotor::IsBackward()
{
  return (Power * PowerScale < 0);
}

uint8_t DCMotor::GetPwmPower()
{
  int8_t ScaledPower = GetScaledPower();
  if (ScaledPower < 0)
    return map(ScaledPower, -128, 0, 255, 0);
  else
    return map(ScaledPower, 0, 127, 0, 255);
}

int8_t DCMotor::GetScaledPower()
{
  return Power * GetScaling();
}

float DCMotor::GetScaling()
{
  return PowerScale;
}

void EatDelimiter()
{
  if (Connection.available())
  {
    char c;
    do
    {
      c = Connection.read();
    } while (c != CommandDelimiter);
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

void loop()
{
  LeftMotor.Stop();
  RightMotor.Stop();

  if (Connection.available())
  {
    char Command;

    Command = Connection.read();

    switch(Command)
    {
      case CommandSetDirection:
      {
        /*
          Command format:

            <CommandSetDirection> <[-100, 100]> ";"
        */

        int8_t Direction = Connection.parseInt();

        EatDelimiter();

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

        Serial.print("kL = ");
        Serial.print(kL);
        Serial.print("; ");
        Serial.print("kR = ");
        Serial.print(kR);
        Serial.println();

        LeftMotor.SetScaling(kL);
        RightMotor.SetScaling(kR);

        PrintMotorsStatus();

        break;
      }
      case CommandSetPower:
      {
        /*
          Command format:

            <CommandSetPower> <[-100, 100]> ";"
        */

        int16_t CmdPower = Connection.parseInt();

        EatDelimiter();

        CmdPower = constrain(CmdPower, -100, 100);

        float z;
        z = (float) CmdPower / 100;
        z *= PI / 2;
        z = sin(z);
        z *= 100;

        CmdPower = z;

        LeftMotor.SetDirectedPower(CmdPower);
        RightMotor.SetDirectedPower(CmdPower);

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
