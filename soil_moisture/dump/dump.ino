const int measurments_delay = 1000;

void print_dryness_sensor(int id, int v)
{
  Serial.print("(id: ");
  Serial.print(id);
  Serial.print(", value: ");
  Serial.print(v);
  Serial.println(")");
}

int read_dryness(int sensor_pin)
{
  int sensor_value = analogRead(sensor_pin);
  return sensor_value;
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  print_dryness_sensor(A0, read_dryness(A0));
  print_dryness_sensor(A1, read_dryness(A1));
  print_dryness_sensor(A2, read_dryness(A2));
  print_dryness_sensor(A3, read_dryness(A3));
  delay(measurments_delay);
}

/*
  2016-02-19
*/



