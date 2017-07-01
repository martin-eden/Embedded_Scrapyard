// Measures soil dryness and pours if needed.

/*
  Status: stable, long term test (8 months) passed
  Last mod.: 2017-03-24
  Notes:
    Further experimenting toward tough "industry" design. New plywood
    box for hardware created.
      * Design is (face view)
        |~|roof
        |-|motor deck
        |_|base

        All decks is equal, and symmetric. This is very please
        constructing and experimenting activities.

    Differences from [flower_friend]:
      * now we serve two motors (up from one),
      * two soil humidity sensors (up from one),
      * no more RGB LED (was nice for eye debugging),
      * no more light intensity sensor (maybe I return it or will use
        real-time clock)

    To do:
      * implement logic: the more is difference from previous result,
        the sooner next measurment should occur. (measurment frequency
        is reverse dependent of signal change)
      * measure with random time intervals (to counter possible
        pattern influencies)
*/

const int num_blocks = 2;

struct t_measurer_params
{
  int sensor_pin;
  int min_value;
  int max_value;
  bool power_off_between_measures;
  bool high_means_dry;
  int power_pin;
};

const t_measurer_params sensor_params[num_blocks] =
  {
    {A1, -1, -1, true, false, 6},
    {A0, -1, -1, true, false, 7}
  }
;

const int motor_pins[num_blocks] = {2, 3};

class humidity_measurer
{
  private:
  public:
    int sensor_pin;
    int min_value;
    int max_value;
    bool power_off_between_measures;
    bool high_means_dry;
    int power_pin;
    humidity_measurer();
    int get_raw_value();
    int get_value();
    bool is_line_problem;
};

const uint32_t line_problem_threshold = 10;

humidity_measurer::humidity_measurer()
{
  sensor_pin = A0;
  power_pin = 2;
  min_value = 0;
  max_value = 680;
  power_off_between_measures = false;
  high_means_dry = false;
  is_line_problem = false;
}

int humidity_measurer::get_raw_value()
{
  int raw_value;
  if (power_off_between_measures)
  {
    digitalWrite(power_pin, HIGH);
    delay(250);
    raw_value = analogRead(sensor_pin);
    digitalWrite(power_pin, LOW);
  }
  else
  {
    raw_value = analogRead(sensor_pin);
  }

  is_line_problem = (raw_value <= line_problem_threshold);

  return raw_value;
}

const int aligned_low = 0;
const int aligned_high = 100;
int humidity_measurer::get_value()
{
  int result;
  result = get_raw_value();
  result = constrain(result, min_value, max_value);
  /*
    Linear mapping [min_value; max_value_segment] to [0, 100].
    Maybe user-defined non-linear mapping should be implemented.
  */
  if (high_means_dry)
    result = map(result, min_value, max_value, aligned_high, aligned_low);
  else
    result = map(result, min_value, max_value, aligned_low, aligned_high);
  return result;
}

humidity_measurer measurer[num_blocks];

class c_motor
{
  public:
    bool is_on;
    int signal_pin;
    void init();
    void start();
    void stop();
};

void c_motor::init()
{
  pinMode(signal_pin, OUTPUT);
  stop();
}

void c_motor::start()
{
  digitalWrite(signal_pin, LOW);
  is_on = true;
}

void c_motor::stop()
{
  digitalWrite(signal_pin, HIGH);
  is_on = false;
}

c_motor motor[num_blocks];

uint32_t cur_time;
uint32_t last_print_time;

void setup()
{
  Serial.begin(9600);

  for (int i = 0; i < num_blocks; i++)
  {
    motor[i].signal_pin = motor_pins[i];
    motor[i].init();
  }

  for (int i = 0; i < num_blocks; i++)
  {
    if (sensor_params[i].sensor_pin != -1)
    {
      pinMode(sensor_params[i].sensor_pin, INPUT);
      measurer[i].sensor_pin = sensor_params[i].sensor_pin;
    }
    if (sensor_params[i].min_value != -1)
      measurer[i].min_value = sensor_params[i].min_value;
    if (sensor_params[i].max_value != -1)
      measurer[i].max_value = sensor_params[i].max_value;
    measurer[i].power_off_between_measures = sensor_params[i].power_off_between_measures;
    measurer[i].high_means_dry = sensor_params[i].high_means_dry;
    if (sensor_params[i].power_pin != -1)
    {
      pinMode(sensor_params[i].power_pin, OUTPUT);
      measurer[i].power_pin = sensor_params[i].power_pin;
    }
  }

  last_print_time = millis();
}

unsigned long min_debug_message_gap_ms = 100;

const int pour_on_percent = 70;
const int pour_off_percent = 80;
const uint32_t idle_measurment_delay = uint32_t(100) * 1000;
const uint32_t pour_measurment_delay = uint32_t(5) * 1000;
uint32_t next_request_time[num_blocks];

void print_status()
{
  if (cur_time - last_print_time > min_debug_message_gap_ms)
  {
    Serial.println(cur_time);
    Serial.println(idle_measurment_delay);
    for (int i = 0; i < num_blocks; i++)
    {
      Serial.print("block: ");
      Serial.print(i);
      Serial.print(", sensor: ");
      Serial.print(measurer[i].get_value());
      Serial.print(" (");
      Serial.print(measurer[i].get_raw_value());
      Serial.print(")");

      Serial.print(", is_line_problem: ");
      Serial.print(measurer[i].is_line_problem);

      Serial.print(", motor: ");
      Serial.print(motor[i].is_on);
      Serial.print("\n\r");
    }
  }

  last_print_time = cur_time;
}

void do_business()
{
  for (int block = 0; block < num_blocks; block++)
  {
    if (
      (cur_time >= next_request_time[block]) ||
      (
        (cur_time < 0x80000000) &&
        (next_request_time[block] >= 0x80000000)
      )
    )
    {
      int val = measurer[block].get_value();
      if (measurer[block].is_line_problem)
      {
        if (motor[block].is_on)
          motor[block].stop();
      }
      else
      {
        if ((val <= pour_on_percent) && (!motor[block].is_on))
        {
          motor[block].start();
          // print_status();
        }
        if ((val >= pour_off_percent) && (motor[block].is_on))
        {
          motor[block].stop();
          // print_status();
        }
      }

      if (motor[block].is_on)
        next_request_time[block] = cur_time + pour_measurment_delay;
      else
        next_request_time[block] = cur_time + idle_measurment_delay;
    }
  }
}

void loop()
{
  cur_time = millis();
  do_business();

  if (Serial.available())
  {
    char cmd = Serial.read();

    switch (cmd)
    {
      case 'A':
        motor[0].start();
        break;
      case 'B':
        motor[1].start();
        break;
      case 'a':
        motor[0].stop();
        break;
      case 'b':
        motor[1].stop();
        break;
      default:
        print_status();
    }
  }
}

/*
  2016-03-16
  2016-03-30
  2016-03-31
  2016-04-21
  2016-04-28
  2016-05-27
  2016-10-11
  2017-01-05
*/
