// Motor part code

/*
  Status: stable
  Last mod.: 2016-05-02
*/

/*
  To do:
    * Implement engines shutdown when some time (say 3 secs) passed
      from last command received. As frequency commands send are
      much higher (now delay between commands is 50 ms so max
      frequency is up to 20 Hz), not receiving commands means
      connection lost and platform is running somewhere undirected.
*/

/*
  As I am going to try joystick for rover navigation, previous serial
  protocol no longer suitable. It proved great for wiring testing and
  simpicity (all you need is just a text terminal on computer/phone).

  New protocol will assume that engines keep rotating. Forecoming
  commands correct their rate. (Some failsafe engine stop is assumed
  when no commands got for some seconds.)

  Also packet format is now dynamically determined by command
  signature.

  (Checksum part is assumed at end. For now, it is just a XOR byte of
  command signature and payload. (Initial value of XOR byte is 00.))

  <command> <command_data> ... <xor_byte>

  <command>:
    00 - set motors speed/direction

      <command_data>:
        <left_motor_speed_direction> <right_motor_speed_direction>

        - this is signed bytes. Zero means drift.

    01 - left motor brake. I.e. rotor is blocked by enabling
      rotation in both sides. (Motor shield claims it can do it.)
    02 - left motor drift
    03 - right motor brake
    04 - right motor drift
      This may be impemented as separate push buttons on board.
    05 - horn on
    06 - horn off

    .. - for further extensions. Unknown commands will be ignored,
    no XOR byte awaited. So for input stream like:

      5A ED 07 02 02 53 00 5A ED B7 51
      -------- ----- -- ----------- --
      ignored  l.motor drift        ignored
                     ignored
                        5A (forward, ~70%) for left motor
                        ED (backward, ~15%) for right motor
                        B7 - 00 XOR 5A XOR ED - command correct

                                                          2016-04-27
*/

struct t_motor_pins
{
  int power;
  int forward;
  int backward;
};

class c_motor
{
  public:
    t_motor_pins pins;
    void init();
    void forward(int percent);
    void backward(int percent);
    void drift();
    void brake();
};

const int max_motor_power = int(1.0 * 255);
// ^ for my case (6v motors and 16v power) it makes sense to limit max duty cycle
// ^^ installed step down divider tuned to 12 volts

void c_motor::init()
{
  pinMode(pins.power, OUTPUT);
  pinMode(pins.forward, OUTPUT);
  pinMode(pins.backward, OUTPUT);
  drift();
}

void c_motor::forward(int percent)
{
  percent = constrain(percent, 0, 100);
  int power = map(percent, 0, 100, 0, max_motor_power);
  digitalWrite(pins.forward, HIGH);
  digitalWrite(pins.backward, LOW);
  analogWrite(pins.power, power);
}

void c_motor::backward(int percent)
{
  percent = constrain(percent, 0, 100);
  int power = map(percent, 0, 100, 0, max_motor_power);
  digitalWrite(pins.backward, HIGH);
  digitalWrite(pins.forward, LOW);
  analogWrite(pins.power, power);
}

void c_motor::brake()
{
  int power = max_motor_power;
  digitalWrite(pins.forward, HIGH);
  digitalWrite(pins.backward, HIGH);
  analogWrite(pins.power, power);
}

void c_motor::drift()
{
  digitalWrite(pins.forward, LOW);
  digitalWrite(pins.backward, LOW);
  analogWrite(pins.power, 0);
}

class c_horn
{
  public:
    int signal_pin;
    void init();
    void start();
    void stop();
};

void c_horn::init()
{
  pinMode(signal_pin, OUTPUT);
}

void c_horn::start()
{
  /*const int horn_freq = 700;
  tone(signal_pin, horn_freq);*/
  digitalWrite(signal_pin, HIGH);
}

void c_horn::stop()
{
  // noTone(signal_pin);
  digitalWrite(signal_pin, LOW);
}

struct t_command
{
  byte code;
  int8_t data[2];
};

const t_motor_pins motor_pins[2] =
  {
    {10, 3, 2},
    {11, 12, 7}
  }
;

const int horn_pin = 8;

c_motor motor[2];
c_horn horn;

void setup()
{
  motor[0].pins = motor_pins[0];
  motor[0].init();
  motor[1].pins = motor_pins[1];
  motor[1].init();
  horn.signal_pin = horn_pin;
  horn.init();
  Serial.begin(9600);
}

#include "rover_commands.h"

const byte state_read_cmd_code = 0;
const byte state_read_data_0 = 1;
const byte state_read_data_1 = 2;
const byte state_read_xor_byte = 3;
const byte state_complete = 4;

t_command get_command()
{
  t_command result;
  byte xor_byte_awaited;
  byte xor_byte_read;
  byte state = state_read_cmd_code;
  byte cur_byte;
  while (state != state_complete)
  {
    if (Serial.available())
    {
      cur_byte = Serial.read();
      switch(state)
      {
        case state_read_cmd_code:
          if (cur_byte == cmd_motor_speeds)
          {
            result.code = cur_byte;
            state = state_read_data_0;
          }
          else if (
            (cur_byte == cmd_left_motor_brake) ||
            (cur_byte == cmd_left_motor_drift) ||
            (cur_byte == cmd_right_motor_brake) ||
            (cur_byte == cmd_right_motor_drift) ||
            (cur_byte == cmd_horn_on) ||
            (cur_byte == cmd_horn_off)
          )
          {
            result.code = cur_byte;
            state = state_read_xor_byte;
          }
          else
            state = state_read_cmd_code; //nothing changed, read next
          break;
        case state_read_data_0:
          result.data[0] = cur_byte;
          state = state_read_data_1;
          break;
        case state_read_data_1:
          result.data[1] = cur_byte;
          state = state_read_xor_byte;
          break;
        case state_read_xor_byte:
          xor_byte_read = cur_byte;
          xor_byte_awaited = result.code;
          if (result.code == cmd_motor_speeds)
          {
            xor_byte_awaited = xor_byte_awaited ^ result.data[0] ^ result.data[1];
          }

          if (ignore_xor_byte)
            xor_byte_read = xor_byte_awaited; //verification was disabled for initial testing

          if (xor_byte_awaited != xor_byte_read)
            state = state_read_cmd_code;
          else
            state = state_complete;

          break;
      }
    }
  }
  return result;
}

void send_motor_data(c_motor a_motor, int8_t motor_data)
{
  byte percent = map(abs(motor_data), 0, 128, 0, 100);
  if (motor_data < 0)
    a_motor.backward(percent);
  else
    a_motor.forward(percent);
}

void execute_command(t_command command)
{
  switch(command.code)
  {
    case cmd_motor_speeds:
      send_motor_data(motor[0], command.data[0]);
      send_motor_data(motor[1], command.data[1]);
      break;
    case cmd_left_motor_brake:
      motor[0].brake();
      break;
    case cmd_left_motor_drift:
      motor[0].drift();
      break;
    case cmd_right_motor_brake:
      motor[1].brake();
      break;
    case cmd_right_motor_drift:
      motor[1].drift();
      break;
    case cmd_horn_on:
      horn.start();
      break;
    case cmd_horn_off:
      horn.stop();
      break;
  }
}

void do_business()
{
  t_command command;
  command = get_command();
  execute_command(command);
}

void loop()
{
  do_business();
}

/*
2016-04-27
2016-04-28
2016-05-02
2016-05-30
*/
