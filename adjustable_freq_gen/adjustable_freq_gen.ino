// Testing program-generated code for timer1 pin 10 toggle.

/*
  Status: stable
  Last mod.: 2017-09-11
*/

/*
  Frequency is depends of A0 value.

  Wiring:

    A0 - potentiometer
    9 - CLK
    <dir_pin> - DIR


  Checked settings:

    Stepper motor with full steps (200 ticks per turn):

      max_delay = 1500
      min_delay = 245
*/

/*
  Delays are in ticks.
  One tick in N * 1/16 mcrs.
  N - prescaler value (8 in this setup).
*/
const uint16_t max_delay = 1200;
const uint16_t min_delay = 30;

const uint16_t min_speed = min_delay / min_delay;
const uint16_t max_speed = max_delay / min_delay;

const uint8_t dir_pin = 8;

void setup_timer_1()
{
  pinMode(9, OUTPUT);
  pinMode(dir_pin, OUTPUT);

  uint8_t sreg;
  sreg = SREG;
  cli();

  TCCR1A &= B01111100;
  TCCR1A |= B01000000;
  TCCR1B &= B11101010;
  TCCR1B |= B00001010;
  TIMSK1 |= B00000010;
  OCR1A = max_delay;

  SREG = sreg;
}

void setup()
{
  setup_timer_1();
}

uint16_t new_ocr1a;
int8_t last_dir = 0;

void loop()
{
  uint16_t cur_analog_value;
  uint16_t cur_speed;

  cur_analog_value = analogRead(A0);
  if (cur_analog_value <= 490)
  {
    if (last_dir != -1)
    {
      digitalWrite(dir_pin, HIGH);
      TCCR1A &= B01111100;
      TCCR1A |= B01000000;
      last_dir = -1;
    }
    cur_speed = map(cur_analog_value, 0, 490, max_speed, min_speed);
  }
  else if (cur_analog_value >= 530)
  {
    if (last_dir != 1)
    {
      digitalWrite(dir_pin, LOW);
      TCCR1A &= B01111100;
      TCCR1A |= B01000000;
      last_dir = 1;
    }
    cur_speed = map(cur_analog_value, 530, 1023, min_speed, max_speed);
  }
  else
  {
    if (last_dir != 1)
    {
      last_dir = 0;
      TCCR1A &= B00111100;
    }
  }

  new_ocr1a = max_delay / cur_speed;

  delay(20);
}

ISR(TIMER1_COMPA_vect)
{
  OCR1A = new_ocr1a;
}
