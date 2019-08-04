#pragma once
#include <Arduino.h>

const byte cmd_motor_speeds = 'M';
const byte cmd_left_motor_brake = 'L';
const byte cmd_left_motor_drift = 'l';
const byte cmd_right_motor_brake = 'R';
const byte cmd_right_motor_drift = 'r';
const byte cmd_horn_on = 'H';
const byte cmd_horn_off = 'h';

const byte cmd_remote_calibrate = 'C';
const byte cmd_remote_discard_calibration = 'c';
const byte cmd_remote_steering_mode_1 = 'T'; // T means tank
const byte cmd_remote_steering_mode_2 = 'J'; // J means joystick
const byte cmd_remote_motors_off = '-';
const byte cmd_remote_motors_on = '+';
const byte cmd_remote_motors_brake = 'B';
const byte cmd_remote_skid_turn_right = '>';
const byte cmd_remote_skid_turn_left = '<';
const byte cmd_remote_forward = '^';
const byte cmd_remote_backward = '_';

const bool ignore_xor_byte = false;

/*
const byte cmd_motor_speeds = 00;
const byte cmd_left_motor_brake = 01;
const byte cmd_left_motor_drift = 02;
const byte cmd_right_motor_brake = 03;
const byte cmd_right_motor_drift = 04;
const byte cmd_horn_on = 05;
const byte cmd_horn_off = 06;
*/
