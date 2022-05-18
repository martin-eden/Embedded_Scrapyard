#pragma once

#include <Arduino.h>
#include <me_DigitalSignalRecorder.h>

namespace me_IrNecParser
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

  enum class FrameType
  {
    Unknown,
    Data,
    ShortRepeat,
    LongRepeat
  };

  class Parser
  {
    public:
      Parser(me_DigitalSignalRecorder* aDSR);

      uint16_t Address;
      uint8_t Command;
      bool HasShortRepeat;
      bool IsRepeat;

      bool Get();

    private:
      uint32_t LastEventTime;
      me_DigitalSignalRecorder* DSR;

      void ConsumeTillStartOfFrame();
      FrameType GetFrameType();
      RecordType GetRecordType(uint16_t Idx);
      bool ConsumeFrameType();

      void Clear();

      bool ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand);
      bool ConsumeByte(uint8_t* pByte);
  };
}
