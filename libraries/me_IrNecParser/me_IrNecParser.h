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

  enum class FrameType
  {
    Unknown,
    Data,
    ShortRepeat,
    LongRepeat
  };

  class me_IrNecParser
  {
    public:
      me_IrNecParser(me_DigitalSignalRecorder* aDSR);

      me_DigitalSignalRecorder* DSR;
      uint16_t Address;
      uint8_t Command;
      bool HasShortRepeat;
      bool IsRepeat;

      bool Get();

    private:
      void ConsumeTillStartOfFrame();
      FrameType GetFrameType();
      RecordType GetRecordType(uint16_t Idx);
      bool ConsumeFrameType();

      bool IsGoodToGo();

      void Clear();

      bool ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand);
      bool ConsumeByte(uint8_t* pByte);
  };
}
