#include "me_DHT11.h"

me_DHT11::me_DHT11(uint8_t aDataPin)
{
  SetDataPin(aDataPin);
}

void me_DHT11::SetDataPin(uint8_t aDataPin)
{
  DataPin = aDataPin;
  PinBitmask = digitalPinToBitMask(DataPin);
  PinPort = digitalPinToPort(DataPin);
}

uint8_t me_DHT11::Read(float* HumidityPtr, float* TemperaturePtr)
{
  uint8_t Result;
  uint16_t HumidityWordPtr, TemperatureWordPtr;

  Result = ReadRaw(&HumidityWordPtr, &TemperatureWordPtr);
  *HumidityPtr = ((int16_t) HumidityWordPtr) / 0x100;
  float TemperatureIntPart = (TemperatureWordPtr & 0xFF00) >> 8;
  // OMG, BCD for low nibble represents tenths of temperature:
  float TemperatureFracPart = (float) (TemperatureWordPtr & 0x000F) / 10;
  bool IsNegativeTemp = (TemperatureIntPart < 0);
  TemperatureIntPart = abs(TemperatureIntPart);
  *TemperaturePtr = TemperatureIntPart + TemperatureFracPart;
  if (IsNegativeTemp)
    *TemperaturePtr = -*TemperaturePtr;

  if ((*TemperaturePtr == 0.0) && (*HumidityPtr == 0.0))
    return me_DHT11_ErrZeroSamples;

  return Result;
}

uint8_t me_DHT11::ReadRaw(uint16_t* HumidityPtr, uint16_t* TemperaturePtr)
{
  uint8_t Result;
  uint8_t Data[40] = {0};

  Result = Sample(Data);
  if (Result == me_DHT11_OK)
    Result = Convert(Data, HumidityPtr, TemperaturePtr);

  return Result;
}

uint8_t me_DHT11::Sample(uint8_t Data[40])
{
  memset(Data, 0, 40);

  pinMode(DataPin, OUTPUT);
  digitalWrite(DataPin, LOW);
  delay(20);

  digitalWrite(DataPin, HIGH);
  pinMode(DataPin, INPUT);

  delayMicroseconds(25);

  uint32_t t = LevelTime(LOW);
  if (t < 30)
    return me_DHT11_CompileError(t, me_DHT11_ErrStartLow);

  t = LevelTime(HIGH);
  if (t < 50)
    return me_DHT11_CompileError(t, me_DHT11_ErrStartHigh);

  for (uint8_t j = 0; j < 40; j++)
  {
    t = LevelTime(LOW);
    if (t < 24)
      return me_DHT11_CompileError(t, me_DHT11_ErrDataLow);

    t = LevelTime(HIGH);
    if (t < 11)
      return me_DHT11_CompileError(t, me_DHT11_ErrDataRead);

    if (t > 40)
      Data[j] = 1;
    else
      Data[j] = 0;
  }

  t = LevelTime(LOW);
  if (t < 24)
    return me_DHT11_CompileError(t, me_DHT11_ErrDataEOF);

  return me_DHT11_OK;
}

uint32_t me_DHT11::LevelTime(uint8_t Level, uint8_t FirstWait, uint8_t Interval)
{
  uint32_t StartTime = micros();
  uint32_t CurTime = micros();

  uint8_t PortState = Level ? PinBitmask : 0;

  bool IsOriginalLevel = true;
  bool IsFirstIteration = true;
  uint32_t TimePassed = CurTime - StartTime;

  while (IsOriginalLevel && (TimePassed <= LevelTimeout))
  {
    if (IsFirstIteration)
    {
      delayMicroseconds(FirstWait);
      IsFirstIteration = false;
    }
    else
    {
      delayMicroseconds(Interval);
    }

    CurTime = micros();
    TimePassed = CurTime - StartTime;

    IsOriginalLevel = ((*portInputRegister(PinPort) & PinBitmask) == PortState);
  }

  if (TimePassed > LevelTimeout)
    return 0;

  return TimePassed;
}

uint8_t me_DHT11::BitsToByte(uint8_t Bits[8])
{
  uint8_t Result = 0;
  for (uint8_t i = 0; i < 8; i++)
    Result |= Bits[i] << (7 - i);
  return Result;
}

uint8_t me_DHT11::Convert(uint8_t Data[40], uint16_t* HumidityPtr, uint16_t* TemperaturePtr)
{
  uint8_t HumidityHighByte = BitsToByte(Data);
  uint8_t HumidityLowByte = BitsToByte(Data + 8);
  uint8_t TemperatureHighByte = BitsToByte(Data + 16);
  uint8_t TemperatureLowByte = BitsToByte(Data + 24);
  uint8_t Checksum = BitsToByte(Data + 32);

  uint8_t ExpectedChecksum =
    HumidityHighByte + HumidityLowByte + TemperatureHighByte + TemperatureLowByte;

  if (Checksum != ExpectedChecksum)
    return me_DHT11_ErrDataChecksum;

  *HumidityPtr = (HumidityHighByte << 8) | HumidityLowByte;
  *TemperaturePtr = (TemperatureHighByte << 8) | TemperatureLowByte;

  return me_DHT11_OK;
}

