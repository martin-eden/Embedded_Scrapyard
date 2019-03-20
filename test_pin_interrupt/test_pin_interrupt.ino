uint8_t interrupt_pin = 2;
uint8_t piezo_pin = 13;

uint16_t beep_fq = 950;
uint16_t beep_duration = 30;
uint16_t beep_min_silence = 20;

volatile bool do_beep = false;

void setup()
{
  pinMode(interrupt_pin, INPUT_PULLUP);
  pinMode(piezo_pin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), handle, RISING);
}

void handle()
{
  if (!do_beep)
    do_beep = true;
}

void loop()
{
  if (do_beep)
  {
    tone(piezo_pin, beep_fq, beep_duration);
    delay(beep_duration + beep_min_silence);
    do_beep = false;
  }
}
