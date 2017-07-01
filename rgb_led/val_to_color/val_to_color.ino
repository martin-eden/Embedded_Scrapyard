// Maps analog value to color palette generated on frame colors

/*
  Status: stable
  Last mod.: 2015-07-08
  Notes: some code beautyfication and generalization won't hurt
*/

const int from_power_to_measurment_delay = 25;
const int measurments_delay = 250;
const int idle_delay = measurments_delay - from_power_to_measurment_delay;

const int power_pin = 8;
const int sensor_pin = A0;

const int red_pin = 11;
const int green_pin = 10;
const int blue_pin = 9;

const int fade_cycles = 128;

// Color arrays
int black_color[3] = {0, 0, 0};
int red_color[3] = {100, 0, 0};
int green_color[3] = {0, 100, 0};
int blue_color[3] = {0, 0, 100};
// int white_color[3] = {100, 100, 100};
// int yellow_color[3] = {40, 95, 0};
// int dim_white_color[3] = {30, 30, 30};

const int sensor_min_value = 0;
const int sensor_max_value = 860;
const int val_to_color_amount = 3 * fade_cycles;

byte val_to_color_r[val_to_color_amount];
byte val_to_color_g[val_to_color_amount];
byte val_to_color_b[val_to_color_amount];

void debug_print_sensor(int v)
{
  Serial.print("(sensor: ");
  Serial.print(v);
  Serial.println(")");
}

void debug_print_color(int r, int g, int b)
{
  Serial.print("(r: ");
  Serial.print(r);
  Serial.print(", g: ");
  Serial.print(g);
  Serial.print(", b: ");
  Serial.print(b);
  Serial.println(")");
}

void debug_print_val_to_color()
{
  for (int i = 0; i < val_to_color_amount; i++)
  {
    analogWrite(red_pin, val_to_color_r[i]);
    analogWrite(green_pin, val_to_color_g[i]);
    analogWrite(blue_pin, val_to_color_b[i]);

    debug_print_color(val_to_color_r[i], val_to_color_g[i], val_to_color_b[i]);

    delay(10);
  }
}

void setup()
{
  pinMode(power_pin, OUTPUT);
  digitalWrite(power_pin, LOW);

  pinMode(sensor_pin, INPUT);

  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);

  int val_to_color_index;

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 0 * fade_cycles + i;
    val_to_color_r[val_to_color_index] = map(i, 0, fade_cycles - 1, black_color[0], red_color[0]);
    val_to_color_g[val_to_color_index] = map(i, 0, fade_cycles - 1, black_color[1], red_color[1]);
    val_to_color_b[val_to_color_index] = map(i, 0, fade_cycles - 1, black_color[2], red_color[2]);
  }

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 1 * fade_cycles + i;
    val_to_color_r[val_to_color_index] = map(i, 0, fade_cycles - 1, red_color[0], green_color[0]);
    val_to_color_g[val_to_color_index] = map(i, 0, fade_cycles - 1, red_color[1], green_color[1]);
    val_to_color_b[val_to_color_index] = map(i, 0, fade_cycles - 1, red_color[2], green_color[2]);
  }

  for (int i = 0; i < fade_cycles; i++)
  {
    val_to_color_index = 2 * fade_cycles + i;
    val_to_color_r[val_to_color_index] = map(i, 0, fade_cycles - 1, green_color[0], blue_color[0]);
    val_to_color_g[val_to_color_index] = map(i, 0, fade_cycles - 1, green_color[1], blue_color[1]);
    val_to_color_b[val_to_color_index] = map(i, 0, fade_cycles - 1, green_color[2], blue_color[2]);
  }

  Serial.begin(9600);
  debug_print_val_to_color();
}

unsigned long last_time = 0;
const long debug_message_delta_ms = 12000;

void loop()
{
  digitalWrite(power_pin, HIGH);
  delay(from_power_to_measurment_delay);
  int sensor_value = analogRead(sensor_pin);
  digitalWrite(power_pin, LOW);

  int index =
    map(
      constrain(
        sensor_value,
        sensor_min_value,
        sensor_max_value
      ),
      sensor_min_value,
      sensor_max_value,
      0,
      val_to_color_amount - 1
    );
  int red_value = val_to_color_r[index];
  int green_value = val_to_color_g[index];
  int blue_value = val_to_color_b[index];

  analogWrite(red_pin, red_value);
  analogWrite(green_pin, green_value);
  analogWrite(blue_pin, blue_value);

  unsigned long cur_time = millis();
  if (cur_time - last_time >= debug_message_delta_ms)
  {
    debug_print_sensor(sensor_value);
    debug_print_color(red_value, green_value, blue_value);
    last_time = cur_time;
  }

  delay(idle_delay);
}
