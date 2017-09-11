// Testing program-generated code for timer1 pin 10 toggle.

/*
  Status: stable
  Last mod.: 2017-09-07
*/

/*
  Frequency is depends of A0 value. Connect potentiometer there.
*/

/*
  Delays are in ticks.
  One tick in N * 1/16 mcrs.
  N - prescaler value (8 in this setup).
*/
const uint16_t max_delay = 1500;
const uint16_t min_delay = 245;

const uint16_t min_speed = (uint32_t)1000000 / max_delay;
const uint16_t max_speed = (uint32_t)1000000 / min_delay;

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
  OCR1A = max_delay;

  SREG = sreg;
}

void setup()
{
  setup_timer_1();
}

void loop()
{
  uint16_t cur_analog_value;
  uint16_t cur_speed;
  uint8_t sreg;
  uint16_t new_ocr1a;
  uint16_t cur_ocr1a;

  cur_analog_value = analogRead(A0);
  cur_speed = map(cur_analog_value, 0, 1023, min_speed, max_speed);

  new_ocr1a = (uint32_t)1000000 / cur_speed;

  while(TCNT1 > 10);
  sreg = SREG;
  cli();
  OCR1A = new_ocr1a;
  SREG = sreg;
}
