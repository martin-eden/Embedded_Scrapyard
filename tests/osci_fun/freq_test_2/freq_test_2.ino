const
  uint8_t OutputPin = 3;

void setup()
{
  pinMode(OutputPin, OUTPUT);
  analogWrite(OutputPin, 512);
}

void loop()
{
}
