#include <SoftwareSerial.h>

const uint8_t
  RightMotorPower_pin = 3,
  RightMotorForward_pin = 4,
  RightMotorBackward_pin = 5,
  LeftMotorPower_pin = 9,
  LeftMotorForward_pin = 11,
  LeftMotorBackward_pin = 10,
  ConnectionRX_pin = 12,
  ConnectionTX_pin = 13;

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
  int Power;
  int Forward;
  int Backward;
};

const t_motor_pins
  LeftMotorPins =
    {
      Power: LeftMotorPower_pin,
      Forward: LeftMotorForward_pin,
      Backward: LeftMotorBackward_pin
    },
  RightMotorPins =
    {
      Power: RightMotorPower_pin,
      Forward: RightMotorForward_pin,
      Backward: RightMotorBackward_pin
    };

class DCMotor
{
  public:
    DCMotor(t_motor_pins motorPins);

    void Stop();

    void SetDirectedPower(int8_t DirectedPower);
    int8_t GetDirectedPower();
    void SetScaling(float scale);
    float GetScaling();

  private:
    t_motor_pins pins;
    uint8_t Power = 0;
    bool IsBackward = false;
    float PowerScale;

    void Actualize();
    void SetPower();
    uint8_t GetScaledPinPower();
    void SetDirection();
};

DCMotor LeftMotor(LeftMotorPins);
DCMotor RightMotor(RightMotorPins);

DCMotor::DCMotor(t_motor_pins motorPins)
{
  this->pins = motorPins;

  pinMode(pins.Power, OUTPUT);
  pinMode(pins.Forward, OUTPUT);
  pinMode(pins.Backward, OUTPUT);

  Stop();
  SetScaling(1.0);
}

void DCMotor::Stop()
{
  Power = 0;
  IsBackward = false;

  Actualize();
}

void DCMotor::SetScaling(float scale)
{
  PowerScale = scale;

  PowerScale = constrain(PowerScale, -1.0, 1.0);

  IsBackward = (PowerScale < 0);

  Actualize();
}

void DCMotor::SetDirectedPower(int8_t DirectedPower)
{
  DirectedPower = constrain(DirectedPower, -100, 100);

  Power = abs(DirectedPower);
  IsBackward = (DirectedPower * PowerScale < 0);

  Actualize();
}

void DCMotor::Actualize()
{
  SetPower();
  SetDirection();
}

void DCMotor::SetPower()
{
  analogWrite(pins.Power, GetScaledPinPower());
}

uint8_t DCMotor::GetScaledPinPower()
{
  uint8_t pinPower;

  pinPower = Power;
  pinPower = map(pinPower, 0, 100, 0, 255);
  pinPower = pinPower * abs(GetScaling());

  return pinPower;
}

float DCMotor::GetScaling()
{
  return PowerScale;
}

void DCMotor::SetDirection()
{
  if (IsBackward)
  {
    digitalWrite(pins.Forward, LOW);
    digitalWrite(pins.Backward, HIGH);
  }
  else
  {
    digitalWrite(pins.Forward, HIGH);
    digitalWrite(pins.Backward, LOW);
  }
}

int8_t DCMotor::GetDirectedPower()
{
  int8_t result;

  result = Power;
  if (IsBackward)
    result = -result;

  return result;
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
  Connection.print(AdjustPowerToGauge(LeftMotor.GetDirectedPower() * LeftMotor.GetScaling()));
  Connection.print("*");

  Connection.print("*R");
  Connection.print(AdjustPowerToGauge(RightMotor.GetDirectedPower() * RightMotor.GetScaling()));
  Connection.print("*");
}

uint16_t AdjustPowerToGauge(int8_t power)
{
  uint16_t result;

  result = power + 100;
  result = min(result, 200);

  return result;
}

void loop()
{
  static bool IsOffState = true;
  static float kL = 1.0;
  static float kR = 1.0;

  if (Connection.available())
  {
    char Command;

    Command = Connection.read();

    switch(Command)
    {
      case CommandOn:
        IsOffState = false;
        break;

      case CommandOff:
        LeftMotor.Stop();
        RightMotor.Stop();
        IsOffState = true;
        break;

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
            Direction = -25: kL = 0.5, kR = 1.0

          Turn left:
            Direction = -50: kL = 0.0, kR = 1.0

          Skid turn left:
            Direction = -100: kL = -1.0, kR = 1.0

        */

        if (Direction < 0)
        {
          kL = map(Direction, -100, 0, -100, 100);
          kL /= 100.0;
          kR = 1.0;
        }
        else
        {
          kR = map(Direction, 0, 100, 100, -100);
          kR /= 100.0;
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

        break;
      }
      case CommandSetPower:
        if (!IsOffState)
        {
          /*
            Command format:

              <CommandSetPower> <[-100, 100]> ";"
          */

          int16_t CmdPower = Connection.parseInt();

          EatDelimiter();

          CmdPower = constrain(CmdPower, -100, 100);

          LeftMotor.SetDirectedPower(CmdPower);
          RightMotor.SetDirectedPower(CmdPower);
        }

        break;

      default:
        Serial.print("Unknown command: \"");
        Serial.print(Command);
        Serial.print("\"");
        Serial.println();
    }
  }

  delay(ProcessingTick_ms);

  if (!IsOffState)
    PrintMotorsStatus();
}
