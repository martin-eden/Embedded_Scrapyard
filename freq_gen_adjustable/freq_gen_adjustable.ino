// Testing program-generated code for timer1 pin 10 toggle.

/*
  Status: stable
  Last mod.: 2017-11-10
*/

/*
  To do:
    Set margins in hertz. Print current frequency.
*/

/*
  Frequency is depends of A0 value.

  Wiring:

    A0 - potentiometer
    9 - CLK
*/

/*
  Delays are in ticks.
  One tick in N * 1/16 mcrs.
  N - prescaler value (8 in this setup).
*/
const uint16_t max_delay = 1200;
const uint16_t min_delay = 30;

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
  TIMSK1 |= B00000010;
  OCR1A = max_delay;

  SREG = sreg;
}

void setup()
{
  setup_timer_1();
  Serial.begin(9600);
}

uint16_t new_ocr1a;
int8_t last_dir = 0;
int16_t prev_analog_value = 0;
const int16_t freq_change_delta = 30;

void loop()
{
  int16_t cur_analog_value;
  uint16_t cur_speed;
  int16_t d;

  cur_analog_value = analogRead(A0);
  d = abs(cur_analog_value - prev_analog_value);
  if (d >= freq_change_delta)
  {
    new_ocr1a = map(cur_analog_value, 0, 1023, min_delay, max_delay);
    prev_analog_value = cur_analog_value;
  }

  Serial.print(cur_analog_value);
  Serial.print(" ");
  Serial.print(prev_analog_value);
  Serial.print(" ");
  Serial.print(d);
  Serial.print(" ");
  Serial.println(new_ocr1a);

  delay(200);
}

ISR(TIMER1_COMPA_vect)
{
  OCR1A = new_ocr1a;
}
