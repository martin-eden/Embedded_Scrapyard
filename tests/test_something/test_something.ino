template <typename type> int8_t sign(type value)
{
  return type((value > 0) - (value < 0));
}

void doSomething()
{
  int8_t Power = -100;
  uint8_t MotorPower;

  MotorPower = map(abs(Power), 0, 100, 0, 255);

  Serial.print("Power: ");
  Serial.print(Power);
  Serial.println();

  Serial.print("MotorPower: ");
  Serial.print(MotorPower);
  Serial.println();
}

void setup()
{
  Serial.begin(57600);

  doSomething();
}

void loop()
{
}
