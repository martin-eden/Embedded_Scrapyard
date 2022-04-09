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

      me_DigitalSignalRecorder* DSR;
      uint16_t Address;
      uint8_t Command;
      bool HasShortRepeat;
      uint16_t NumRepeats;

      bool Get();

    private:
      RecordType GetRecordType(uint32_t Pause, uint32_t Signal);
      RecordType GetHistoryRecType(uint16_t Idx);
      void Clear();
      bool ConsumeDataFrameHeader();
      bool ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand);
      bool ConsumeByte(uint8_t* pByte);

      bool ConsumeShortRepeatFrame();
      bool ConsumeLongRepeatFrame();
  };
}
