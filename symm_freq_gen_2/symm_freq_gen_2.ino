// Testing program-generated code for timer1 pin 10 toggle.

/*
  Status: stable
  Last mod.: 2017-09-07
*/

void setup_timer_1()
{
  pinMode(9, OUTPUT);

  uint8_t sreg;
  sreg = SREG;
  cli();

  TCCR1A &= B01111100;
  TCCR1A |= B01000000;
  TCCR1B &= B11101010;
  TCCR1B |= B00001010;
  OCR1A = 9999;

  SREG = sreg;
}

void setup()
{
  setup_timer_1();
}

void loop()
{
  // See dear, I'm generating sound with empty loop()!
}
