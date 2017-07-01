// Remote IR switch.

/*
  Status: stable
  Last mod.: 2016-06-30
*/

/*
  I've used Omron solid-state relay. My model switches silently
  but works only with AC and not suitable for light dimming
  (it flips too slow, maybe about 10-20 Hz. Such blinking light
  is annoying.).
  --
  All Omron G3MP-202P solid-state relays I've used broke in
  about two months of usage for upper light (220V AC 180W).
  This is quite strange. Some lamps which it was connected
  several times instantly flashed when relay was off with
  a instant bluelish flash (like a lighting) and got burned
  too.

  So I'm returned to electromechanical relays.
*/

#include <IRremote.h>

const uint8_t ir_in_pin = 12;
const uint8_t led_pin = 13;
const uint8_t relay_state_pin = 6;

IRrecv irrecv(ir_in_pin);

void setup()
{
  // Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(led_pin, OUTPUT);
  pinMode(relay_state_pin, OUTPUT);
}

void set_state(int8_t switch_state)
{
  if (switch_state)
  {
    digitalWrite(led_pin, HIGH);
    digitalWrite(relay_state_pin, HIGH);
  }
  else
  {
    digitalWrite(led_pin, LOW);
    digitalWrite(relay_state_pin, LOW);
  }
}

int8_t switch_state = 0;

uint32_t last_signal_time = 0;
uint32_t next_signal_delay_ms = 175;

void debug_print_state(uint32_t cur_time, uint32_t time_passed)
{
  Serial.print("cur_time: ");
  Serial.print(cur_time);
  Serial.print(", last_signal_time: ");
  Serial.print(last_signal_time);
  Serial.print(", time_passed: ");
  Serial.print(time_passed);
  Serial.println("");
}

void flip()
{
  uint32_t cur_time = millis();
  uint32_t time_passed = cur_time - last_signal_time;
  // debug_print_state(cur_time, time_passed);
  if (time_passed > next_signal_delay_ms)
  {
    switch_state = switch_state ^ 1;
    set_state(switch_state);
  }
  last_signal_time = cur_time;
}

/*
  "keyes" is Keyes remote with unnamed model (with CR-2025 battery).
  Code format NEC.
*/
const uint32_t keyes_ok = 0xFF02FD;
void handle_nec_code(unsigned long code)
{
  switch(code)
  {
    case keyes_ok:
      flip();
      break;
  }
}

// "gs" prefix means my GoldStar remote model 105-230D. Code format RC5.
const uint32_t gs_bit_mask = 0x800;
const uint32_t gs_power = 0xC;

void handle_rc5_code(uint32_t code)
{
  code = code & ~gs_bit_mask;
  switch(code)
  {
    case gs_power:
      flip();
      break;
  }
}

decode_results results;

void loop()
{
  if (irrecv.decode(&results))
  {
    if (!results.overflow)
    {
      // Serial.println(results.value, HEX);
      if (results.decode_type == NEC)
        handle_nec_code(results.value);
      else if (results.decode_type == RC5)
        handle_rc5_code(results.value);
    }
    irrecv.resume();
  }
}
