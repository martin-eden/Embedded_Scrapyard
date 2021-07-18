#include <Arduino.h>
#include "me_RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(uint8_t _PhaseA_pin, uint8_t _PhaseB_pin, uint8_t _Switch_pin) {
  PhaseA_pin = _PhaseA_pin;
  PhaseB_pin = _PhaseB_pin;
  Switch_pin = _Switch_pin;

  pinMode(PhaseA_pin, INPUT_PULLUP);
  pinMode(PhaseB_pin, INPUT_PULLUP);
  pinMode(Switch_pin, INPUT_PULLUP);

  CurState = GetPinsState();
  UpdateState();
  PositionHasChanged = true;

  SwitchState = GetSwitchState();
  LastSwitchTime_ms = 0;
  UpdateSwitch();
  SwitchHasChanged = true;
}

int32_t RotaryEncoder::GetPosition() {
  int32_t result;
  uint8_t sreg = SREG;
  cli();
  result = Position;
  SREG = sreg;
  return result;
}

uint8_t RotaryEncoder::GetState(bool PhaseA, bool PhaseB) {
  if (!PhaseA & !PhaseB)
    return 0;
  else if (!PhaseA & PhaseB)
    return 1;
  else if (PhaseA & PhaseB)
    return 2;
  else if (PhaseA & !PhaseB)
    return 3;
  else
    return 100;
}

uint8_t RotaryEncoder::GetPinsState() {
  return GetState(digitalRead(PhaseA_pin), digitalRead(PhaseB_pin));
}

const int8_t UnreachableState = 50;

int8_t StateDelta[4][4] = {
  {0, 1, UnreachableState, -1},
  {-1, 0, 1, UnreachableState},
  {UnreachableState, -1, 0, 1},
  {1, UnreachableState, -1, 0}
};

void RotaryEncoder::UpdateState() {
  PrevState = CurState;
  CurState = GetPinsState();

  int8_t StateDelta_value = StateDelta[CurState][PrevState];
  if (StateDelta_value == UnreachableState)
    CurState = PrevState;
  else
    Position += StateDelta_value;

  if (CurState != PrevState)
    PositionHasChanged = true;
}

bool RotaryEncoder::GetSwitchState() {
  return digitalRead(Switch_pin);
}

void RotaryEncoder::UpdateSwitch() {
  PrevSwitchState = SwitchState;
  SwitchState = GetSwitchState();
  int32_t CurTime_ms = millis();
  if (SwitchState != PrevSwitchState) {
    if (CurTime_ms - LastSwitchTime_ms > Debounce_ms) {
      LastSwitchTime_ms = CurTime_ms;
      SwitchHasChanged = true;
    }
  }
}
