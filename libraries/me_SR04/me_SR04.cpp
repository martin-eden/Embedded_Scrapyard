// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.2
  Last mod.: 2022-10-08
*/

/*
  This sensor registers echo returned after emitting pulse signal.

  Unlike normal physics, to get distance we need to measure not delay
  between start of pulse and start of echo but the duration of signal
  on echo pin.

  After emitting strobe, there is some delay (0.5, 2) ms depending of
  flair of SR-04. Then echo pin goes HIGH. It stays high till start
  of receiving of echo. Or until timeout which is like 200 ms.
*/

#include "me_SR04.h"
#include <me_SignalTime.h>

using namespace me_SR04;
using namespace me_SignalTime;

void me_SR04::SR04::Request()
{
  const uint32_t
    StrobeDurationMcr = 250,
    SignalStartTimeoutMcr = 2500,
    SignalMaxDurationMcr = 50000,
    SignalEndTimeoutMcr = 250000;

  uint32_t SignalStartMcr;
  uint32_t SignalDurationMcr;

  RequestStatus = ReadStatus::Unknown;

  pinMode(TriggerPin, OUTPUT);

  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(StrobeDurationMcr);
  digitalWrite(TriggerPin, LOW);

  pinMode(EchoPin, INPUT);

  SignalStartMcr = GetLevelTime(EchoPin, LOW, SignalStartTimeoutMcr);
  if (SignalStartMcr == 0)
  {
    RequestStatus = ReadStatus::NoSignalStart;
  }
  else
  {
    SignalDurationMcr = GetLevelTime(EchoPin, HIGH, SignalEndTimeoutMcr);
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
