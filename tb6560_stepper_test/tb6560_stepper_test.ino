const uint8_t clock_pin = 2;
const uint8_t direction_pin = 4;
const uint8_t enable_pin = 6;

uint32_t clock_pulse_width_mcrs = 600;

void setup()
{
  pinMode(clock_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);
  pinMode(enable_pin, OUTPUT);
}

void tic()
{
  digitalWrite(clock_pin, HIGH);
  delayMicroseconds(clock_pulse_width_mcrs);
  digitalWrite(clock_pin, LOW);
  delayMicroseconds(clock_pulse_width_mcrs);
}

int8_t d = -1;

void loop()
{
  for (int i = 0; i < 200; i++)
  {
    for (int k = 0; k < 5; k++)
      tic();
    clock_pulse_width_mcrs += d;
    if ((clock_pulse_width_mcrs < 175) || (clock_pulse_width_mcrs > 800))
      d = -d;
  }
}

// 2017-06-18
