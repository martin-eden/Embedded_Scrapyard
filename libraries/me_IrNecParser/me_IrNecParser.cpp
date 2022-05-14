// IR NEC format parser

/*
  Status: stable
  Version: 1.0
  Last mod.: 2022-04-24
*/

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
  LastEventTime = 0;
}

/*
  Parse DSRs queue.

  NEC data format is:

    FrameHeader ─┬─ Address ─ Command ─┬─
                 ├─ ShortRepeat ───────┤
                 ╰─ LongRepeat ────────╯

  Logical sequence is

    DataFrame [ShortRepeat] (LongRepeat)*

  Short and long repeats are just one signal pulse. They contain no data
  and indicate that previously pressed button is still pressed.

  Difference between short and long repeats is pause duration before
  pulse. For short repeat it is near 40ms, for long - near 100ms. Short
  repeat is often emitted after data just in case. So short repeat
  should not be treated as new key press.

  This function returns <true> if able to parse <DataFrame> or
  <LongRepeat>. When it's unable to parse data or data starts with
  <ShortRepeat>, it returns <false>.

  In process it removes all unrecognized data before <FrameHeader>,
  <FrameHeader> itself and elements data.
*/
bool me_IrNecParser::Get()
{
  if (!DSR->HasEvents())
    return false;

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
        if (GetFrameType() == FrameType::ShortRepeat)
        {
          ConsumeFrameType();
          HasShortRepeat = true;
        }

        IsRepeat = false;
        while (GetFrameType() == FrameType::LongRepeat)
        {
          ConsumeFrameType();
          IsRepeat = true;
        }

        if (DSR->Queue.IsEmpty())
          DSR->Clear();

        LastEventTime = micros();
      }

      return result;
    }

    case FrameType::LongRepeat:
      if (micros() - LastEventTime <= 150000)
      {
        IsRepeat = true;
        LastEventTime = micros();
        return true;
      }
      return false;

    case FrameType::ShortRepeat:
    case FrameType::Unknown:
    default:
      return false;
  }
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
  if (!Cursor.Next())
    return Result;
  uint16_t SecondRecIdx = Cursor.Get();

  if (GetRecordType(FirstRecIdx) != RecordType::FrameHeader)
    return Result;

  RecordType recordType = GetRecordType(SecondRecIdx);

  if (recordType == RecordType::DataFrame)
    Result = FrameType::Data;
  else if (recordType == RecordType::RepeatFrame)
  {
    uint32_t PauseDuration = DSR->History[FirstRecIdx].Pause;
    if (
      IsWithin(PauseDuration, 95000, 105000) ||
      (PauseDuration == 0)
    )
      Result = FrameType::LongRepeat;
    else if (IsWithin(PauseDuration, 39000, 45000))
      Result = FrameType::ShortRepeat;
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
