// SQW through writing to PORT-register. Effective frea is around 6 MHz.
// Output pin is A0 which is PORTC.Bit[0].

const
  uint8_t OutputPin = A0;

void setup()
{
  pinMode(OutputPin, OUTPUT);
  digitalWrite(OutputPin, LOW);
}

void loop()
{
  for (uint32_t i = 0; true; ++i)
  {
    PORTC = 0;
    PORTC = 1;
    PORTC = 0;
    PORTC = 1;
    PORTC = 0;
    PORTC = 1;
    PORTC = 0;
    PORTC = 1;
    PORTC = 0;
    PORTC = 1;
    PORTC = 0;
    PORTC = 1;
  }
}
