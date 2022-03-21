#pragma once

#include <Arduino.h>
#include <me_DigitalSignalRecorder.h>

namespace IrNecParser
{
  enum class RecordType
    {
      Unknown,
      FrameHeader,
      RepeatFrame,
      DataFrame,
      BitZero,
      BitOne
    };

  class me_IrNecParser
  {
    public:
      me_IrNecParser(me_DigitalSignalRecorder* aDSR);
      uint16_t Address;
      uint8_t Command;
      bool Get();
      me_DigitalSignalRecorder* DSR;

    private:
      RecordType GetRecordType(uint32_t Pause, uint32_t Signal);
      RecordType GetHistoryRecType(uint16_t Idx);
      void ConsumeTillStartOfFrame();
      void ConsumeRepeatFrames();
      bool ConsumeDataFrameHeader();
      bool ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand);
      bool ConsumeByte(uint8_t* pByte);
  };
}
