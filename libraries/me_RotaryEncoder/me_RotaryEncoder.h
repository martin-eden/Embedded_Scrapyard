// Rotary encoder tracking

/*
  Status: done
  Version: 1.0
  Last mod.: 2021-04-28
*/

/*
  Rotary encoder model: KY-040

  Tracking done via hardware interrupts on two phase pins. For button press
  software polling is used, as there are just two hardware interrupt pins
  available on Arduino Uno. Button press event is filtered by debouncing.

  Wiring:

    CLK (PhaseA_pin) => 2 or 3
    DT (PhaseB_pin) => 3 or 2
    SW (Switch_pin) => any pin

  Usage:

    RotaryEncoder RotaryEncoder(PhaseA_pin, PhaseB_pin, Switch_pin);
    RotaryEncoder.Debounce_ms = Debounce_ms;
    attachInterrupt(digitalPinToInterrupt(PhaseA_pin), Phase_change, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PhaseB_pin), Phase_change, CHANGE);

    void Phase_change() {
      RotaryEncoder.UpdateState();
    }

    cli();
    int32_t Position_copy = RotaryEncoder.Position;
    sei();
    if (RotaryEncoder.PositionHasChanged) {
      Process(Position_copy);
      ...
      RotaryEncoder.PositionHasChanged = false;
    }

    RotaryEncoder.UpdateSwitch();
    if (RotaryEncoder.SwitchHasChanged) {
      Process(RotaryEncoder.SwitchState);
      ...
      RotaryEncoder.SwitchHasChanged = false;
    }
*/

#pragma once

class RotaryEncoder {
  public:
    RotaryEncoder(uint8_t _PhaseA_pin, uint8_t _PhaseB_pin, uint8_t _Switch_pin);
    uint8_t PhaseA_pin;
    uint8_t PhaseB_pin;
    uint8_t Switch_pin;
    uint32_t Debounce_ms = 40;

    bool PositionHasChanged = false;
    int32_t GetPosition();

    bool SwitchState;
    bool SwitchHasChanged = false;

    void UpdateState();
    void UpdateSwitch();
  private:
    int32_t Position = 0;
    uint8_t CurState, PrevState;
    bool PrevSwitchState;
    int32_t LastSwitchTime_ms;

    uint8_t GetState(bool PhaseA, bool PhaseB);
    uint8_t GetPinsState();
    bool GetSwitchState();
};
