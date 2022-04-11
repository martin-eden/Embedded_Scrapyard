#include "me_IrNecParser.h"

#include <me_QueueMindEnumerator.h>

using namespace IrNecParser;

me_IrNecParser::me_IrNecParser(me_DigitalSignalRecorder* aDSR)
{
  DSR = aDSR;
  Clear();
}

void me_IrNecParser::Clear()
{
  Address = 0;
  Command = 0;
  HasShortRepeat = false;
  IsRepeat = false;
}

/*
  Parse DSRs queue.

  General NEC data representation in queue:

    FrameHeader Address Command [ShortRepeat] (RepeatFrame)*

  Sets following fields:

    Address
    Command
    HasShortRepeat
    NumRepeats

  If DSRs history starts with repeat frame, we count that
  repeat frames, and keep previous command.
*/
bool me_IrNecParser::Get()
{
  if (!IsGoodToGo())
  {
    if (DSR->HasEvents())
    {
      // Serial.println("Not good to go.");
      // Serial.println(micros() - DSR->GetLastEventTime());
    }

    return false;
  }

  ConsumeTillStartOfFrame();

  FrameType frameType = GetFrameType();
  ConsumeFrameType();

  switch (frameType)
  {
    case FrameType::Data:
    {
      uint16_t tempAddress;
      uint8_t tempCommand;

      bool result = ConsumeDataFrame(&tempAddress, &tempCommand);

      if (result)
      {
        Address = tempAddress;
        Command = tempCommand;
        HasShortRepeat = false;
        IsRepeat = false;
      }

      return result;
    }
    case FrameType::ShortRepeat:
    {
      HasShortRepeat = true;

      return true;
    }
    case FrameType::LongRepeat:
    {
      IsRepeat = true;

      return true;
    }
    case FrameType::Unknown:
    default:
    {
      DSR->Clear();

      return false;
    }
  }
}

bool me_IrNecParser::IsGoodToGo()
{
  const uint32_t MinimalDelayToProcess = 65000;

  return
    DSR->IsFull() ||
    (
      DSR->HasEvents() &&
      (micros() - DSR->GetLastEventTime() >= MinimalDelayToProcess)
    );
}

void me_IrNecParser::ConsumeTillStartOfFrame()
{
  while (
    !DSR->Queue.IsEmpty() &&
    (GetFrameType() == FrameType::Unknown)
  )
  {
    DSR->Queue.Dequeue();
  }
}

bool IsWithin(uint32_t CurValue, uint32_t MinValue, uint32_t MaxValue)
{
  return ((CurValue >= MinValue) && (CurValue <= MaxValue));
}

FrameType me_IrNecParser::GetFrameType()
{
  FrameType Result = FrameType::Unknown;

  if (DSR->Queue.IsEmpty())
    return Result;

  me_QueueMindEnumerator Cursor(&DSR->Queue);

  uint16_t FirstRecIdx = Cursor.Get();

  if (!Cursor.Move())
    return Result;

  uint16_t SecondRecIdx = Cursor.Get();

  if (GetRecordType(FirstRecIdx) != RecordType::FrameHeader)
    return Result;

  switch (GetRecordType(SecondRecIdx))
  {
    case RecordType::DataFrame:
    {
      Result = FrameType::Data;
      break;
    }
    case RecordType::RepeatFrame:
    {
      uint32_t PauseDuration = DSR->History[FirstRecIdx].Pause;
      if (
        IsWithin(PauseDuration, 95000, 105000) ||
        (PauseDuration == 0)
      )
      {
        Result = FrameType::LongRepeat;
      }
      else if (IsWithin(PauseDuration, 39000, 45000))
      {
        Result = FrameType::ShortRepeat;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return Result;
}

RecordType me_IrNecParser::GetRecordType(uint16_t Idx)
{
  uint32_t PauseDuration = DSR->History[Idx].Pause;
  uint32_t SignalDuration = DSR->History[Idx].Signal;

  const uint32_t
    // 562.5 actually but I dont want to introduce floats
    BaseDuration = 562;

  if (IsWithin(SignalDuration, 8500, 9500))
    return RecordType::FrameHeader;

  RecordType Result = RecordType::Unknown;

  /*
    For some reason, Pause-Singal durations may vary considerably from
    base duration of 562.5. But their sum remains close to multiple of
    that value.

      Real data: Pause = 672, Signal = 476.
      Delta: Pause = 109.5, Signal = -86.5.
      Sum: 672 + 476 == 1148.
      Sum delta: = 1148 % 562.5 = 23.

    So we are using sum, rounding it to nearest <BaseDuration> multiple
    and making decision based of number of <BaseDuration>'s inside
    that sum.
  */

  uint32_t TotalDuration = PauseDuration + SignalDuration;
  uint32_t DeltaOvershoot = TotalDuration % BaseDuration;
  uint32_t DeltaUndershoot = BaseDuration - DeltaOvershoot;

  bool IsGoodDelta =
    IsWithin(DeltaOvershoot, 0, 120) ||
    IsWithin(DeltaUndershoot, 0, 120);

  if (IsGoodDelta)
  {
    if (DeltaOvershoot < DeltaUndershoot)
      TotalDuration -= DeltaOvershoot;
    else
      TotalDuration += DeltaUndershoot;

    uint32_t Multiplicity = TotalDuration / BaseDuration;

    switch (Multiplicity)
    {
      case 5:
        Result = RecordType::RepeatFrame;
        break;
      case 9:
        Result = RecordType::DataFrame;
        break;
      case 2:
        Result = RecordType::BitZero;
        break;
      case 4:
        Result = RecordType::BitOne;
        break;
      default:
        break;
    }
  }

  return Result;
}

bool me_IrNecParser::ConsumeFrameType()
{
  return DSR->Queue.Dequeue() && DSR->Queue.Dequeue();
}

bool me_IrNecParser::ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand)
{
  uint8_t RawData[4];

  for (uint8_t i = 0; i < 4; ++i)
    if (!ConsumeByte(&RawData[i]))
    {
      return false;
    }

  *oAddress = (RawData[0] << 8) | RawData[1];

  if (RawData[2] != (uint8_t) ~RawData[3])
  {
    return false;
  }

  *oCommand = RawData[2];

  return true;
}

bool me_IrNecParser::ConsumeByte(uint8_t* pByte)
{
  *pByte = 0;

  for (uint8_t i = 0; i < 8; ++i)
  {
    if (DSR->Queue.IsEmpty())
      return false;

    uint8_t BitValue;

    switch (GetRecordType(DSR->Queue.GetFirstIdx()))
    {
      case RecordType::BitZero:
        BitValue = 0;
        break;
      case RecordType::BitOne:
        BitValue = 1;
        break;
      default:
        return false;
        break;
    }

    *pByte |= (BitValue << i);

    DSR->Queue.Dequeue();
  }

  return true;
}
