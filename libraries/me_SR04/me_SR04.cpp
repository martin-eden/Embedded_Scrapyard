// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.1
  Last mod.: 2022-06-20
*/

/*
  This sensor registeres echo returned after emitting pulse signal.

  Unlike normal physics, to get distance we need to measure duration of
  sinal on echo pin, not delay between end of pulse and start of echo.

  After emitting strobe ~10us duration ~500us passes and echo pin goes
  HIGH. In sensor circuit there is builtin echo pin HIGH duration
  timeout ~= 200ms.
*/

#include "me_SR04.h"
#include <me_SignalTime.h>

using namespace me_SR04;
using namespace me_SignalTime;

void me_SR04::SR04::Request()
{
  const uint32_t
    PulseDurationMcr = 250,
    SignalStartTimeoutMcr = 1000,
    SignalMaxDurationMcr = 50000,
    SignalMaxWaitTimeout = 250000;

  uint32_t SignalStartMcr;
  uint32_t SignalDurationMcr;

  RequestStatus = ReadStatus::Unknown;

  pinMode(TriggerPin, OUTPUT);

  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(PulseDurationMcr);
  digitalWrite(TriggerPin, LOW);

  pinMode(EchoPin, INPUT);

  SignalStartMcr = GetLevelTime(EchoPin, LOW, SignalStartTimeoutMcr);
  if (SignalStartMcr == 0)
  {
    RequestStatus = ReadStatus::NoSignalStart;
  }
  else
  {
    SignalDurationMcr = GetLevelTime(EchoPin, HIGH, SignalMaxWaitTimeout);
    if ((SignalDurationMcr == 0) || (SignalDurationMcr > SignalMaxDurationMcr))
    {
      RequestStatus = ReadStatus::NoSignalEnd;
    }
    else
    {
      EchoDelayMcr = SignalDurationMcr;
      RequestStatus = ReadStatus::Success;
    }
  }
}
