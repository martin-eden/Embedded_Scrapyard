#include <me_IrNecParser.h>

me_IrNecParser::me_IrNecParser(me_DigitalSignalRecorder* aDSR)
{
  DSR = aDSR;
}

bool me_IrNecParser::Get()
{
  const uint32_t
    MinimalDelayToProcess = 50000,
    MinimalPacketDuration = 59000;

  if (!DSR->HasEvents() || (micros() - DSR->GetLastEventTime() <= MinimalDelayToProcess))
    return false;

  ConsumeTillStartOfFrame();

  while (ConsumeRepeatFrame());

  uint16_t tempAddress;
  uint8_t tempCommand;

  bool result = ConsumeDataFrame(&tempAddress, &tempCommand);

  if (result)
  {
    while (ConsumeRepeatFrame());

    Address = tempAddress;
    Command = tempCommand;
  }

  DSR->Clear();

  return result;
}

bool IsWithin(uint32_t CurValue, uint32_t MinValue, uint32_t MaxValue)
{
  return ((CurValue >= MinValue) && (CurValue <= MaxValue));
}

const uint32_t
  DataFrameSignalDuration = 9000,
  DataFramePauseDuration = 4500,

  RepeatFramePause1Duration = 40000,
  RepeatFrameSignalDuration = 9000,
  RepeatFramePause2Duration = 2250,

  BitZeroPause = 562,
  BitOnePause = 1687,
  BitAnySignal = 562;

void me_IrNecParser::ConsumeTillStartOfFrame()
{
  while (!DSR->Queue.IsEmpty())
  {
    uint8_t CurIdx = DSR->Queue.GetFirstIdx();
    uint32_t Signal = DSR->History[CurIdx].Signal;
    if (IsWithin(Signal, DataFrameSignalDuration - 500, DataFrameSignalDuration + 500))
      break;
    DSR->Queue.RemoveFirst();
  }
}

bool me_IrNecParser::ConsumeRepeatFrame()
{
  if (DSR->Queue.GetNumElements() < 2)
    return false;

  uint8_t CurIdx = DSR->Queue.GetFirstIdx();

  uint32_t Signal = DSR->History[CurIdx].Signal;

  CurIdx = DSR->Queue.GetNextIdx(CurIdx);

  uint32_t Pause = DSR->History[CurIdx].Pause;

  if (
    IsWithin(Signal, RepeatFrameSignalDuration - 500, RepeatFrameSignalDuration + 500) &&
    IsWithin(Pause, RepeatFramePause2Duration - 100, RepeatFramePause2Duration + 100)
  )
  {
    DSR->Queue.RemoveFirst();
    DSR->Queue.RemoveFirst();
    return true;
  }
  return false;
}

bool me_IrNecParser::ConsumeDataFrame(uint16_t* oAddress, uint8_t* oCommand)
{
  uint8_t CurIdx = DSR->Queue.GetFirstIdx();

  uint32_t Signal = DSR->History[CurIdx].Signal;

  CurIdx = DSR->Queue.GetNextIdx(CurIdx);

  uint32_t Pause = DSR->History[CurIdx].Pause;

  CurIdx = DSR->Queue.GetNextIdx(CurIdx);

  if (
    IsWithin(Signal, DataFrameSignalDuration - 500, DataFrameSignalDuration + 500) &&
    IsWithin(Pause, DataFramePauseDuration - 100, DataFramePauseDuration + 100)
    )
  {
    DSR->Queue.RemoveFirst();
    DSR->Queue.RemoveFirst();

    uint8_t RawData[4];

    for (uint8_t i = 0; i < 4; ++i)
      if (!ConsumeByte(&RawData[i]))
        return false;

    *oAddress = (RawData[0] << 8) | RawData[1];

    if (RawData[2] != (uint8_t) ~RawData[3])
      return false;
    *oCommand = RawData[2];

    return true;
  }
  return false;
}

bool me_IrNecParser::ConsumeByte(uint8_t* pByte)
{
  *pByte = 0;

  for (uint8_t i = 0; i < 8; ++i)
  {
    if (DSR->Queue.IsEmpty())
      return false;
    uint8_t CurIdx = DSR->Queue.GetFirstIdx();

    uint32_t Pause = DSR->History[CurIdx].Pause;
    uint32_t Signal = DSR->History[CurIdx].Signal;

    if (!IsWithin(Signal, BitAnySignal - 100, BitAnySignal + 100))
      return false;

    uint8_t BitValue;
    if (IsWithin(Pause, BitZeroPause - 100, BitZeroPause + 100))
      BitValue = 0;
    else if (IsWithin(Pause, BitOnePause - 100, BitOnePause + 100))
      BitValue = 1;
    else
      return false;

    *pByte |= (BitValue << i);

    DSR->Queue.RemoveFirst();
  }

  // Serial.println(*pByte, HEX);

  return true;
}
