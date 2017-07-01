/*
  Status: in writing
  Last mod.: 2016-12-19
*/

/*
  Counts number of state changes.
*/

const int interrupt_pin = 2;
volatile uint32_t num_calls = 0;
volatile uint32_t last_interrupt_time;
volatile uint32_t cur_interrupt_time;

const uint32_t min_interrupt_step_mcrs = 2000;

void count()
{
  if (cur_interrupt_time - last_interrupt_time >= min_interrupt_step_mcrs)
  {
    num_calls++;
  }
  last_interrupt_time = cur_interrupt_time;
}

int32_t last_millis = 0;
uint32_t last_num_calls = 0;

void setup()
{
  pinMode(interrupt_pin, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(interrupt_pin), count, RISING);
  attachInterrupt(0, count, RISING);
  last_millis = millis();
  last_interrupt_time = micros();

  Serial.begin(9600);
}

int32_t update_step_ms = 3000;
void loop()
{
  cur_interrupt_time = micros();

  static int32_t cur_millis;
  cur_millis = millis();
  int32_t time_passed_ms = cur_millis - last_millis;
  if (time_passed_ms >= update_step_ms)
  {
    static float rpm;
    rpm = (num_calls - last_num_calls) / ((0.0 + time_passed_ms) / 60000);
    //Serial.println(num_calls - last_num_calls);
    last_millis = cur_millis;
    last_num_calls = num_calls;
    Serial.println(rpm);
  }
}



