// Measures soil dryness and pours if needed.

/*
  Status: stable
  Last mod.: 2016-02-28
  Notes: some code beautyfication and generalization won't hurt
*/

/*
  Also uses light sensor and RGB LED.
  Pouring occurs if RGB LED is on.
  RGB LED is on when there is dark enough.
*/

const int measurments_delay = 3000;
const long debug_message_delta_ms = measurments_delay;

/*
  Dark intensity thresholds for "on" and "off".
  Some hysteresis gap is made to prevent blinking at
  border values.
*/
const int light_off_threshold = 540;
const int light_on_threshold = 570;

const int dryness_power_pin = 8;
const int dryness_sensor_pin = A0;
const int dark_sensor_pin = A1;
const int red_pin = 11;
const int green_pin = 10;
const int blue_pin = 9;
const int pour_pin = 2;

const int sensor_min_value = 580;
const int sensor_max_value = 640;
const int pour_dryness_threshold = 50; //in percents

struct t_color
{
  byte red;
  byte green;
  byte blue;
};

struct t_color black_color = {0, 0, 0};
struct t_color red_color = {100, 0, 0};
struct t_color green_color = {0, 100, 0};
struct t_color blue_color = {0, 0, 100};
// t_color white_color = {100, 100, 100};
// t_color yellow_color = {40, 95, 0};
// t_color dim_white_color = {30, 30, 30};

const int fade_cycles = 128;
const int val_to_color_amount = 3 * fade_cycles;

struct t_color val_to_color[val_to_color_amount];

void debug_print_light_sensor(int v)
{
  Serial.print("(light_sensor: ");
  Serial.print(v);
  Serial.println(")");
}


void debug_print_color(struct t_color color)
{
  Serial.print("(r: ");
  Serial.print(color.red);
  Serial.print(", g: ");
  Serial.print(color.green);
  Serial.print(", b: ");
  Serial.print(color.blue);
  Serial.println(")");
}


void set_led_color(struct t_color color)
{
  analogWrite(red_pin, color.red);
  analogWrite(green_pin, color.green);
  analogWrite(blue_pin, color.blue);
}


void debug_print_val_to_color()
{
  const int items_print_delay = 10;
  for (int i = 0; i < val_to_color_amount; i++)
  {
    set_led_color(val_to_color[i]);
    debug_print_color(val_to_color[i]);
    delay(items_print_delay);
  }
}


void turn_off_led()
{
  set_led_color(black_color);
}


void pour()
{
  const int pour_duration = 8000;
  Serial.println("Pouring.");
  digitalWrite(pour_pin, LOW);
  delay(pour_duration);
  digitalWrite(pour_pin, HIGH);
}


const int dryness_history_size = 3;
int dryness_prev_values[dryness_history_size];
int dryness_prev_position = 0;
int dryness_num_values = 0;

const int power_to_measurement_delay_ms = 60;
const int line_problem_low_threshold_value = 100;

float read_dryness(int *is_ok)
{
  digitalWrite(dryness_power_pin, HIGH);
  delay(power_to_measurement_delay_ms);
  int sensor_value = analogRead(dryness_sensor_pin);
  digitalWrite(dryness_power_pin, LOW);

  Serial.print("(raw moisture sensor value: ");
  Serial.print(sensor_value);
  Serial.println(")");

  if (sensor_value <= line_problem_low_threshold_value)
  {
    Serial.println("Looks like line is broken. Ignoring value.");
    *is_ok = 0;
    return 0;
  }

  int aligned_sensor_value;
  aligned_sensor_value =
    map(
      constrain(sensor_value, sensor_min_value, sensor_max_value),
      sensor_min_value,
      sensor_max_value,
      0,
      100
    );

  Serial.print("(aligned_sensor_value: ");
  Serial.print(aligned_sensor_value);
  Serial.println(")");

  dryness_prev_position++;
  if (dryness_prev_position >= dryness_history_size)
    dryness_prev_position = 0;
  dryness_prev_values[dryness_prev_position] = aligned_sensor_value;
  if (dryness_num_values < dryness_history_size)
    dryness_num_values++;

  int sum = 0;
  int pos = dryness_prev_position;
  int steps_done = 0;
  while (steps_done < dryness_num_values)
  {
    sum += dryness_prev_values[pos];
    /*
    Serial.print("(pos: ");
    Serial.print(pos);
    Serial.print(", ");
    Serial.print("value: ");
    Serial.print(dryness_prev_values[pos]);
    Serial.print(", ");
    Serial.print("sum: ");
    Serial.print(sum);
    Serial.println(")");
    */
    pos--;
    if (pos < 0)
      pos = dryness_history_size - 1;
    steps_done++;
  }

  if (dryness_num_values == 0)
  {
    Serial.println("No valid data read.");
    *is_ok = 0;
    return 0;
  }
  else
  {
    float avg = (float)sum / dryness_num_values;
    /*
    Serial.print("(dryness_num_values: ");
    Serial.print(dryness_num_values);
    Serial.println(")");
    */
    Serial.print("(result_average: ");
    Serial.print(avg);
    Serial.println(")");

    *is_ok = -1;
    return avg;
  }
}

void setup()
{
  pinMode(pour_pin, OUTPUT);
  digitalWrite(pour_pin, HIGH);

  pinMode(dryness_power_pin, OUTPUT);
  digitalWrite(dryness_power_pin, LOW);

  pinMode(dryness_sensor_pin, INPUT);
  pinMode(dark_sensor_pin, INPUT);

  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  int val_to_color_index;

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 0 * fade_cycles + i;
    val_to_color[val_to_color_index].red = map(i, 0, fade_cycles - 1, black_color.red, red_color.red);
    val_to_color[val_to_color_index].green = map(i, 0, fade_cycles - 1, black_color.green, red_color.green);
    val_to_color[val_to_color_index].blue = map(i, 0, fade_cycles - 1, black_color.blue, red_color.blue);
  }

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 1 * fade_cycles + i;
    val_to_color[val_to_color_index].red = map(i, 0, fade_cycles - 1, red_color.red, green_color.red);
    val_to_color[val_to_color_index].green = map(i, 0, fade_cycles - 1, red_color.green, green_color.green);
    val_to_color[val_to_color_index].blue = map(i, 0, fade_cycles - 1, red_color.blue, green_color.blue);
  }

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 2 * fade_cycles + i;
    val_to_color[val_to_color_index].red = map(i, 0, fade_cycles - 1, green_color.red, blue_color.red);
    val_to_color[val_to_color_index].green = map(i, 0, fade_cycles - 1, green_color.green, blue_color.green);
    val_to_color[val_to_color_index].blue = map(i, 0, fade_cycles - 1, green_color.blue, blue_color.blue);
  }

  Serial.begin(9600);
  // debug_print_val_to_color();
}


unsigned long last_time = 0;
boolean rgb_led_is_on = false;

void loop()
{
  int dryness_sensor_value = -1;
  int dark_sensor_value = analogRead(dark_sensor_pin);
  if (rgb_led_is_on && (dark_sensor_value <= light_off_threshold))
  {
    rgb_led_is_on = false;
    Serial.print(dark_sensor_value);
    Serial.println(" - Light off.");
    turn_off_led();
  }
  else if (!rgb_led_is_on && (dark_sensor_value >= light_on_threshold))
  {
    rgb_led_is_on = true;
    Serial.print(dark_sensor_value);
    Serial.println(" - Light on.");
  }

  int index = 0;
  // if (rgb_led_is_on)
  // {
    int is_ok;
    dryness_sensor_value = read_dryness(&is_ok);
    if (is_ok)
    {
      index =
        map(
          dryness_sensor_value,
          0,
          100,
          0,
          val_to_color_amount - 1
        );
      set_led_color(val_to_color[index]);

      if (dryness_sensor_value <= pour_dryness_threshold)
      {
        pour();
      }
    }
  // }

  unsigned long cur_time = millis();
  if (cur_time - last_time >= debug_message_delta_ms)
  {
    // debug_print_light_sensor(dark_sensor_value);
    // debug_print_color(val_to_color[index]);
    last_time = cur_time;
  }

  delay(measurments_delay);
}

/*
  2015-07-08
  2015-07-28
  2015-08-01
  2015-10-20
  2015-11-08
  2015-12-06
  2015-12-07
  2015-12-17
  2016-01-01
  2016-02-17
  2016-02-28
*/
