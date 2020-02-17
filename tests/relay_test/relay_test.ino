const int signal_pin = 3;
const int work_time_ms = 3000;
const int stop_time_ms = 1500;

void open_gate()
{
  digitalWrite(signal_pin, LOW);
}

void close_gate()
{
  digitalWrite(signal_pin, HIGH);
}

void setup()
{
  pinMode(signal_pin, OUTPUT);
  close_gate();
}

void loop()
{
  open_gate();
  delay(work_time_ms);
  close_gate();
  delay(stop_time_ms);
}


