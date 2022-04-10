#include <me_IrNecParser.h>

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
  NumRepeats = 0;
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

  Clears queue at end regardless of parse result.
*/
bool me_IrNecParser::Get()
{
  const uint32_t
    MinimalDelayToProcess = 125000;

  if (
      !DSR->IsFull() &&
      (
        !DSR->HasEvents() ||
        (micros() - DSR->GetLastEventTime() < MinimalDelayToProcess)
      )
  )
  {
    return false;
  }

  if (ConsumeLongRepeatFrame())
  {
    NumRepeats = 1;
    while(ConsumeLongRepeatFrame())
      ++NumRepeats;
    DSR->Clear();
    return true;
  }

  uint16_t tempAddress;
  uint8_t tempCommand;

  bool result = ConsumeDataFrame(&tempAddress, &tempCommand);

  if (result)
  {
    Address = tempAddress;
    Command = tempCommand;

    HasShortRepeat = ConsumeShortRepeatFrame();

    NumRepeats = 1;
    while (ConsumeLongRepeatFrame())
      ++NumRepeats;
  }
  // else
  //   Serial.println("Not a data frame.");

  DSR->Clear();

  return result;
}

bool me_IrNecParser::ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand)
{
  if (!ConsumeDataFrameHeader())
  {
    // Serial.println("Data frame header not found.");
    return false;
  }

  if (DSR->GetCount() < 32)
  {
    // Serial.println("Too few elements.");
    // Serial.println(DSR->Queue.GetCount());
    return false;
  }

  uint8_t RawData[4];

  for (uint8_t i = 0; i < 4; ++i)
    if (!ConsumeByte(&RawData[i]))
    {
      // Serial.println("ConsumeByte() failed.");
      return false;
    }

  *oAddress = (RawData[0] << 8) | RawData[1];

  if (RawData[2] != (uint8_t) ~RawData[3])
  {
    // Serial.println("Checksum failed.");
    return false;
  }

  *oCommand = RawData[2];

  return true;
}

bool me_IrNecParser::ConsumeDataFrameHeader()
{
  if (DSR->GetCount() < 2)
    return false;

  uint16_t FirstRecIdx = DSR->Queue.GetFirstIdx();
  uint16_t SecondRecIdx = DSR->Queue.GetNextIdx(FirstRecIdx);

  if (
    (GetHistoryRecType(FirstRecIdx) == RecordType::FrameHeader) &&
    (GetHistoryRecType(SecondRecIdx) == RecordType::DataFrame)
  )
  {
    DSR->Queue.Dequeue();
    DSR->Queue.Dequeue();
    return true;
  }

  return false;
}

RecordType me_IrNecParser::GetHistoryRecType(uint16_t Idx)
{
  uint32_t Pause = DSR->History[Idx].Pause;
  uint32_t Signal = DSR->History[Idx].Signal;

  return GetRecordType(Pause, Signal);
}

bool IsWithin(uint32_t CurValue, uint32_t MinValue, uint32_t MaxValue)
{
  return ((CurValue >= MinValue) && (CurValue <= MaxValue));
}

const uint32_t
  LongRepeatDuration = 96000,
  BaseDuration = 562; // 562.5 actually but I dont want to introduce floats

RecordType me_IrNecParser::GetRecordType(uint32_t PauseDuration, uint32_t SignalDuration)
{
  if (IsWithin(SignalDuration, 8500, 9500))
    return RecordType::FrameHeader;

  RecordType Result = RecordType::Unknown;

  /*
    For some reason, Pause-Singal durations may vary considerably from
    base duration of 562.5. But their sum remains close to multiple of
    that value:

      Consider Pause = 672, Signal = 476.
      Delta Pause = 109.5, delta Signal = -86.5.
      Sum = 672 + 476 = 1148.
      Delta Sum = 1148 % 562.5 = 23.
  */

  uint32_t TotalDuration = PauseDuration + SignalDuration;
  uint32_t DeltaOvershoot = TotalDuration % BaseDuration;
  uint32_t DeltaUndershoot = BaseDuration - DeltaOvershoot;

  bool IsGoodDelta =
    IsWithin(DeltaOvershoot, 0, 120) ||
    IsWithin(DeltaUndershoot, 0, 120);

  /*
  if (!IsGoodDelta)
  {
    Serial.println("---");
    Serial.println(PauseDuration);
    Serial.println(SignalDuration);
    Serial.println(TotalDuration);
    Serial.println(DeltaOvershoot);
    Serial.println(DeltaUndershoot);
    Serial.println("===");
  }
  */

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
        // Serial.print("Multiplicity: ");
        // Serial.println(Multiplicity);
        break;
    }
  }

  return Result;
}

bool me_IrNecParser::ConsumeByte(uint8_t* pByte)
{
  *pByte = 0;

  for (uint8_t i = 0; i < 8; ++i)
  {
    if (DSR->Queue.IsEmpty())
      return false;

    uint8_t BitValue;

    switch (GetHistoryRecType(DSR->Queue.GetFirstIdx()))
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

bool me_IrNecParser::ConsumeShortRepeatFrame()
{
  while (DSR->GetCount() >= 2)
  {
    uint16_t FirstRecIdx = DSR->Queue.GetFirstIdx();
    uint16_t SecondRecIdx = DSR->Queue.GetNextIdx(FirstRecIdx);

    if (
      (GetHistoryRecType(FirstRecIdx) == RecordType::FrameHeader) &&
      (GetHistoryRecType(SecondRecIdx) == RecordType::RepeatFrame) &&
      IsWithin(DSR->History[FirstRecIdx].Pause, 39000, 45000)
    )
    {
      DSR->Queue.Dequeue();
      DSR->Queue.Dequeue();

      return true;
    }
    else
      break;
  }

  return false;
}

bool me_IrNecParser::ConsumeLongRepeatFrame()
{
  while (DSR->GetCount() >= 2)
  {
    uint16_t FirstRecIdx = DSR->Queue.GetFirstIdx();
    uint16_t SecondRecIdx = DSR->Queue.GetNextIdx(FirstRecIdx);

    if (
      (GetHistoryRecType(FirstRecIdx) == RecordType::FrameHeader) &&
      (GetHistoryRecType(SecondRecIdx) == RecordType::RepeatFrame) &&
      (
        IsWithin(DSR->History[FirstRecIdx].Pause, 95000, 105000) ||
        (DSR->History[FirstRecIdx].Pause == 0)
      )
    )
    {
      DSR->Queue.Dequeue();
      DSR->Queue.Dequeue();

      return true;
    }
    else
      break;
  }

  return false;
}
