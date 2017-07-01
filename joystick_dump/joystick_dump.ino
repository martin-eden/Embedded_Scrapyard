/*
  Status: tested
  Last mod.: 2016-04-26
*/

const int axis_x_pin = A1;
const int axis_y_pin = A0;
const int switch_pin = 4;

void setup()
{
  pinMode(switch_pin, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop()
{
  int x_value = analogRead(axis_x_pin);
  int y_value = analogRead(axis_y_pin);
  int switch_state = digitalRead(switch_pin);

  Serial.print("x: ");
  Serial.print(x_value);
  Serial.print(", ");
  Serial.print("y: ");
  Serial.print(y_value);
  Serial.print(", ");
  Serial.print("button: ");
  if (switch_state == HIGH)
    Serial.print("OFF");
  else
    Serial.print("ON");
  Serial.print("");
  Serial.print("\n");
  delay(250);
}

