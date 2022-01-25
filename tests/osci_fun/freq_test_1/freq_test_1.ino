// SQW through digitalWrite(). Effective freq is around 550 kHz.

const
  uint8_t OutputPin = A0;

void setup()
{
  pinMode(OutputPin, OUTPUT);
  digitalWrite(OutputPin, LOW);
}

void loop()
{
  digitalWrite(OutputPin, LOW);
  digitalWrite(OutputPin, HIGH);
}
