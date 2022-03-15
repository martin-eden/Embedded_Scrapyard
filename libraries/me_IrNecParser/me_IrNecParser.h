#pragma once

#include <Arduino.h>
#include <me_DigitalSignalRecorder.h>

class me_IrNecParser
{
  public:
    me_IrNecParser(me_DigitalSignalRecorder* aDSR);
    uint16_t Address;
    uint8_t Command;
    bool Get();

  private:
    me_DigitalSignalRecorder* DSR;
    void ConsumeTillStartOfFrame();
    bool ConsumeRepeatFrame();
    bool ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand);
    bool ConsumeByte(uint8_t* pByte);
};
