// Serial interface to control two motors

/*
  Status: done
  Tested on: Arduino Uno
  Last mod.: 2016-04-08
*/

/*
  It looks like a bad idea to continously control engines
  via serial interface. (In my case I have to enter characters
  in terminal on smart phone.) So each command turns engines on
  for a limited period of time.

  Duration time is <N> amount of tenths of second, where <N> is
  <duration> * (10 ^ <time_power>)

  <left_engine_vector> <right_engine_vector> [<duration> [<time_power>]]

  <left_engine_vector> = <vector>
  <right_engine_vector> = <vector>

  <vector> = "A" .. "J", "a" .. "j", " "
    | A - 10% power, forward
    | a - 10% power, backward
    | B - 20% power, forward
    | ...
    | j - 100% power, backward
    | <space> - 0% power

  <duration> = "0" .. "9"
    | 0 - 0
    | 1 - 1
    | ...
    | 9 - 9
    |
    | Default: 1

  <time_power> = "0" .. "8"
    | 0 - 1
    | 1 - 10
    | ...
    | 8 - 100 000 000
    |
    | Default: 0
*/

#define MOTOR_LEFT_POWER_PIN 10
#define MOTOR_LEFT_FORWARD_PIN 4
#define MOTOR_LEFT_BACKWARD_PIN 2

#define MOTOR_RIGHT_POWER_PIN 11
#define MOTOR_RIGHT_FORWARD_PIN 12
#define MOTOR_RIGHT_BACKWARD_PIN 7

#define SERIAL_RATE 9600

#define DEFAULT_DURATION 1
#define DEFAULT_POWER 0

unsigned long tenths_powers[] =
  {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000
  };

void setup()
{
  pinMode(MOTOR_LEFT_POWER_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_FORWARD_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_BACKWARD_PIN, OUTPUT);

  pinMode(MOTOR_RIGHT_POWER_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_FORWARD_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_BACKWARD_PIN, OUTPUT);

  Serial.begin(SERIAL_RATE);
}

void print_help()
{
  Serial.println("Enter from two to four characters: engines vectors and optional duration.");
  Serial.println("");
  Serial.println("Duration time is <N> amount of tenths of second, where <N> is");
  Serial.println("  <duration> * (10 ^ <time_power>)");
  Serial.println("");
  Serial.println("  <left_engine_vector> <right_engine_vector> [<duration> [<time_power>]]");
  Serial.println("");
  Serial.println("  <left_engine_vector> = <vector>");
  Serial.println("  <right_engine_vector> = <vector>");
  Serial.println("  ");
  Serial.println("  <vector> = \"A\" .. \"J\", \"a\" .. \"j\", \" \"");
  Serial.println("    | A - 10\% power, forward");
  Serial.println("    | a - 10\% power, backward");
  Serial.println("    | B - 20\% power, forward");
  Serial.println("    | ...");
  Serial.println("    | j - 100\% power, backward");
  Serial.println("    | <space> - 0\% power");
  Serial.println("");
  Serial.println("  <duration> = \"0\" .. \"9\"");
  Serial.println("    | 0 - 0");
  Serial.println("    | 1 - 1");
  Serial.println("    | ...");
  Serial.println("    | 9 - 9");
  Serial.println("    |");
  Serial.println("    | Default: 1");
  Serial.println("");
  Serial.println("  <time_power> = \"0\" .. \"8\"");
  Serial.println("    | 0 - 1");
  Serial.println("    | 1 - 10");
  Serial.println("    | ...");
  Serial.println("    | 8 - 100 000 000");
  Serial.println("    |");
  Serial.println("    | Default: 0");
}

int char_to_vector(char c)
{
  int result = 0;
  if ((c >= 'A') && (c <= 'J'))
    result = (int(c) - int('A') + 1) * 10;
  else if ((c >= 'a') && (c <= 'j'))
    result = -(int(c) - int('a') + 1) * 10;
  else
  {
    if (c != ' ')
      Serial.println("Failsafe value set.");
    result = 0;
  }
  return result;
}

int char_to_duration(char c)
{
  int result = DEFAULT_DURATION;
  if ((c >= '0') && (c <= '9'))
    result = c - '0';
  result = result * 100;
  return result;
}

unsigned long char_to_power(char c)
{
  unsigned long result = DEFAULT_POWER;
  if ((c >= '0') && (c <= '8'))
    result = c - '0';
  result = tenths_powers[result];
  return result;
}

void execute_command(
  int left_engine_vector,
  int right_engine_vector,
  unsigned long duration
)
{
  int left_engine_power = map(abs(left_engine_vector), 0, 100, 0, 255);
  if (left_engine_vector > 0)
    digitalWrite(MOTOR_LEFT_FORWARD_PIN, HIGH);
  else
    digitalWrite(MOTOR_LEFT_BACKWARD_PIN, HIGH);
  analogWrite(MOTOR_LEFT_POWER_PIN, left_engine_power);

  int right_engine_power = map(abs(right_engine_vector), 0, 100, 0, 255);
  if (right_engine_vector > 0)
    digitalWrite(MOTOR_RIGHT_FORWARD_PIN, HIGH);
  else
    digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, HIGH);
  analogWrite(MOTOR_RIGHT_POWER_PIN, right_engine_power);

  delay(duration);

  digitalWrite(MOTOR_LEFT_POWER_PIN, LOW);
  digitalWrite(MOTOR_LEFT_FORWARD_PIN, LOW);
  digitalWrite(MOTOR_LEFT_BACKWARD_PIN, LOW);

  digitalWrite(MOTOR_RIGHT_POWER_PIN, LOW);
  digitalWrite(MOTOR_RIGHT_FORWARD_PIN, LOW);
  digitalWrite(MOTOR_RIGHT_BACKWARD_PIN, LOW);
}

void loop()
{
  char cmd[4];
  int bytes_read = Serial.readBytesUntil('\n', cmd, 4);
  if (bytes_read >= 2)
  {
    int left_engine_vector = char_to_vector(cmd[0]);
    int right_engine_vector = char_to_vector(cmd[1]);

    unsigned long duration;
    if (bytes_read >= 3)
      duration = char_to_duration(cmd[2]);
    else
      duration = char_to_duration(' ');

    int time_power;
    if (bytes_read >= 4)
      time_power = char_to_power(cmd[3]);
    else
      time_power = char_to_power(' ');

    duration = duration * time_power;

    Serial.print("(");
    // Serial.print("left_engine_vector: ");
    Serial.print(left_engine_vector);
    Serial.print(", ");
    // Serial.print("right_engine_vector: ");
    Serial.print(right_engine_vector);
    Serial.print(", ");
    // Serial.print("duration: ");
    Serial.print(duration);
    Serial.println(")        ");

    execute_command(left_engine_vector, right_engine_vector, duration);
  }
  else if (bytes_read > 0)
  {
    Serial.println("Command too short.");
    if (cmd[0] == '?')
      print_help();
    else
      Serial.println("Type '?' for help.");
  }
}

/*
2015-05-20
2015-05-26
2016-04-08
*/
