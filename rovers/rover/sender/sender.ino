// Control data sender for rover platform

/*
  Status: stable
  Last mod.: 2016-05-06
*/

/*
  Transmits analog data from control board via bluetooth to
  paired receiver on rover.
*/

#include "rover_commands.h"
#include "joystick.h"
#include <SoftwareSerial.h>
#include <Keypad.h>

SoftwareSerial soft_serial(8, 9); // RX, TX

const t_joystick_pins joystick_pins = {A0, A1, 3};
const int discrecity_ms = 50;

c_joystick joystick;

const byte keypad_num_rows = 4;
const byte keypad_num_columns = 4;
/*
  definition order:
    3 2 1 0
    . . . .
    F E C D
  physical order:
    F E C D
    . . . .
    3 2 1 0
*/
char keypad_commands[keypad_num_rows][keypad_num_columns] = {
  {cmd_remote_motors_on, ' ', ' ', cmd_remote_backward},
  {cmd_remote_motors_off, cmd_remote_motors_brake, ' ', cmd_remote_forward},
  {cmd_remote_calibrate, cmd_remote_discard_calibration, cmd_remote_skid_turn_left, cmd_remote_skid_turn_right},
  {cmd_remote_steering_mode_1, ' ', ' ', cmd_remote_steering_mode_2}
};
byte keypad_row_pins[keypad_num_rows] = {12, 11, 10, 7};
byte keypad_column_pins[keypad_num_columns] = {6, 5, 4, 2};

Keypad keypad =
  Keypad(
    makeKeymap(keypad_commands),
    keypad_row_pins,
    keypad_column_pins,
    keypad_num_rows,
    keypad_num_columns
  );

void setup()
{
  joystick.pins = joystick_pins;
  joystick.invert_x = true;
  joystick.invert_y = true;
  joystick.init();
  Serial.begin(9600);
  while (!Serial) ;
  soft_serial.begin(9600);
  Serial.println("Ready to serve.");
}

void send_horn_state(bool is_pressed)
{
  byte xor_byte = 00;
  byte cmd;
  if (is_pressed)
    cmd = cmd_horn_on;
  else
    cmd = cmd_horn_off;
  xor_byte = xor_byte ^ cmd;

  soft_serial.write(cmd);
  soft_serial.write(xor_byte);

  Serial.print(char(cmd));
  Serial.print(" ");
  Serial.print(xor_byte, HEX);
  Serial.print("\n");
}

void engines_off()
{
  byte xor_byte;
  byte cmd;

  cmd = cmd_left_motor_drift;
  xor_byte = cmd;

  soft_serial.write(cmd);
  soft_serial.write(xor_byte);

  Serial.print(char(cmd));
  Serial.print(" ");
  Serial.print(xor_byte, HEX);
  Serial.print("\n");

  cmd = cmd_right_motor_drift;
  xor_byte = cmd;

  soft_serial.write(cmd);
  soft_serial.write(xor_byte);

  Serial.print(char(cmd));
  Serial.print(" ");
  Serial.print(xor_byte, HEX);
  Serial.print("\n");
}

void engines_on()
{
}

void engines_brake()
{
}

void send_motor_directions(int left_motor_percent, int right_motor_percent)
{
  byte cmd;
  int8_t left_motor_vector;
  int8_t right_motor_vector;

  cmd = cmd_motor_speeds;

  left_motor_percent = constrain(left_motor_percent, -100, 100);
  left_motor_vector = map(left_motor_percent, -100, 100, -128, 127);

  right_motor_percent = constrain(right_motor_percent, -100, 100);
  right_motor_vector = map(right_motor_percent, -100, 100, -128, 127);

  byte xor_byte = cmd ^ left_motor_vector ^ right_motor_vector;

  soft_serial.write(cmd);
  soft_serial.write(left_motor_vector);
  soft_serial.write(right_motor_vector);
  soft_serial.write(xor_byte);

  Serial.print(char(cmd));
  Serial.print(" ");
  Serial.print(left_motor_percent, DEC);
  Serial.print(" -> ");
  Serial.print(left_motor_vector, DEC);
  Serial.print(", ");
  Serial.print(right_motor_percent, DEC);
  Serial.print(" -> ");
  Serial.print(right_motor_vector, DEC);
  Serial.print(" ");
  Serial.print(xor_byte, HEX);
  Serial.print("\n");

  Serial.write(cmd);
  Serial.write(left_motor_vector);
  Serial.write(right_motor_vector);
  Serial.write(xor_byte);
  Serial.print("\n");
}

bool last_button_state = false;
byte steering_mode = cmd_remote_steering_mode_1;
byte engine_state = cmd_remote_motors_off;

const int micro_movement_delay_ms = 50;
const int micro_movement_power_perc = 60;

void loop()
{
  char keypad_key = keypad.getKey();
  if (keypad_key && (keypad_key != ' '))
  {
    switch(keypad_key)
    {
      case cmd_remote_calibrate:
        joystick.calibrate();
        send_horn_state(true);
        delay(150);
        send_horn_state(false);
        delay(100);
        send_horn_state(true);
        delay(50);
        send_horn_state(false);
        delay(50);
        send_horn_state(true);
        delay(50);
        send_horn_state(false);
        break;
      case cmd_remote_discard_calibration:
        break;
      case cmd_remote_steering_mode_1:
        steering_mode = cmd_remote_steering_mode_1;
        send_horn_state(true);
        delay(50);
        send_horn_state(false);
        break;
      case cmd_remote_steering_mode_2:
        steering_mode = cmd_remote_steering_mode_2;
        send_horn_state(true);
        delay(50);
        send_horn_state(false);
        delay(100);
        send_horn_state(true);
        delay(50);
        send_horn_state(false);
        break;
      case cmd_remote_motors_off:
        engines_off();
        engine_state = cmd_remote_motors_off;
        send_horn_state(true);
        delay(150);
        send_horn_state(false);
        break;
      case cmd_remote_motors_on:
        engine_state = cmd_remote_motors_on;
        for (int i = 0; i < 3; i++)
        {
          send_horn_state(true);
          delay(50);
          send_horn_state(false);
          delay(50);
        }
        break;
      case cmd_remote_motors_brake:
        break;
      case cmd_remote_skid_turn_right:
        send_motor_directions(micro_movement_power_perc, -micro_movement_power_perc);
        delay(micro_movement_delay_ms);
        send_motor_directions(0, 0);
        break;
      case cmd_remote_skid_turn_left:
        send_motor_directions(-micro_movement_power_perc, micro_movement_power_perc);
        delay(micro_movement_delay_ms);
        send_motor_directions(0, 0);
        break;
      case cmd_remote_forward:
        send_motor_directions(micro_movement_power_perc, micro_movement_power_perc);
        delay(micro_movement_delay_ms);
        send_motor_directions(0, 0);
        break;
      case cmd_remote_backward:
        send_motor_directions(-micro_movement_power_perc, -micro_movement_power_perc);
        delay(micro_movement_delay_ms);
        send_motor_directions(0, 0);
        break;

    }
  }
  bool button_pressed = joystick.is_button_pressed();
  if (button_pressed != last_button_state)
  {
    send_horn_state(button_pressed);
    last_button_state = button_pressed;
  }
  else
    if (engine_state == cmd_remote_motors_on)
    {
      if (steering_mode == cmd_remote_steering_mode_1)
      {
        int x = joystick.get_scaled_x();
        int y = joystick.get_scaled_y();
        send_motor_directions(x, y);
      }
      else if (steering_mode == cmd_remote_steering_mode_2)
      {
        int power = joystick.get_scaled_x();
        int steering_raw_value = joystick.get_scaled_y();
        int steering, left_motor_percent, right_motor_percent;
        if (steering_raw_value < 0)
        {
          steering = map(steering_raw_value, -100, 0, -power, power);
          left_motor_percent = steering;
          right_motor_percent = power;
          send_motor_directions(left_motor_percent, right_motor_percent);
        }
        else
        {
          steering = map(steering_raw_value, 0, 100, power, -power);
          left_motor_percent = power;
          right_motor_percent = steering;
          send_motor_directions(left_motor_percent, right_motor_percent);
        }
      }
    }

  delay(discrecity_ms);

  /*
  if (soft_serial.available())
    Serial.write(soft_serial.read());
  if (Serial.available())
    soft_serial.write(Serial.read());
  */
}

/*
2016-04-28
2016-05-06
*/
