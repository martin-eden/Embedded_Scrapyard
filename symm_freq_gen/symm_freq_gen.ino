// Experiments with Atmega328P timer 1

/*
  Status: stable, experimental
  Last mod.: 2017-07-24
*/

const uint8_t output_pin = 2;

void timer_1_prescale_256()
{
  TCCR1B &= ~(B111 << 0);
  TCCR1B |= B100 << 0;
}

void timer_1_enable_match_a()
{
  TIMSK1 &= ~(B1 << 1);
  TIMSK1 |= B1 << 1;
}

void timer_1_disable_match_a()
{
  TIMSK1 &= ~(B1 << 1);
}

void timer_1_normal()
{
  TCCR1A &= ~(B11 << 0);
  TCCR1B &= ~(B11 << 3);
  TCCR1A |= B00 << 0;
  TCCR1B |= B00 << 3;
}

void timer_1_ctc_by_ocr()
{
  TCCR1A &= ~(B11 << 0);
  TCCR1B &= ~(B11 << 3);
  TCCR1A |= B00 << 0;
  TCCR1B |= B01 << 3;
}

void timer_1_pwm_fast_by_ocr()
{
  TCCR1A &= ~(B11 << 0);
  TCCR1B &= ~(B11 << 3);
  TCCR1A |= B11 << 0;
  TCCR1B |= B11 << 3;
}

const uint32_t chip_frequency = 16000000;
const uint32_t tacts_per_micro = chip_frequency / 1000000;

uint16_t calc_ocr(uint32_t num_micros)
{
  /*
    Equation:

      prescaler * ocr / tacts_per_micro == num_micros

    So

      ocr == num_micros * tacts_per_micro / prescaler

    Fixing <prescaler> to 256 and knowing harware const <tacts_per_micro> 16

      ocr == num_micros / 16
      ocr == num_micros >> 4
  */
  uint32_t wished_ocr = num_micros >> 4;
  if (wished_ocr > 0xFFFF)
    return 0xFFFF;
  if (wished_ocr == 0)
    return 0;
  return (uint16_t) wished_ocr;
}

void set_delay(uint32_t num_micros)
{
  uint16_t ocr = calc_ocr(num_micros);
  uint8_t sreg = SREG;
  cli();
  OCR1A = ocr;
  SREG = sreg;
}

void set_frequency(float hz)
{
  uint32_t num_micros = 1000000 / hz;
  set_delay(num_micros);
}

void setup()
{
  const float init_freq = 10000 * 2;
  uint8_t sreg;

  pinMode(output_pin, OUTPUT);
  set_frequency(init_freq);

  sreg = SREG;
  cli();
  timer_1_enable_match_a();
  timer_1_prescale_256();
  //timer_1_ctc_by_ocr();
  timer_1_pwm_fast_by_ocr();
  //timer_1_normal();
  SREG = sreg;
}

void loop()
{
  // See dear, I'm generating sound with empty loop()!
}

uint8_t state = 0;

ISR(TIMER1_COMPA_vect)
{
  state ^= 1;
  digitalWrite(output_pin, state);
}
