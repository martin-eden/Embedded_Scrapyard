// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.3
  Last mod.: 2022-10-09
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

void SR04::Ping()
{
  RequestStatus = ReadStatus::Unknown;

  Throw();

  bool IsStarted, IsEnded;
  uint32_t Duration;

  Catch(&IsStarted, &IsEnded, &Duration);

  if (!IsStarted)
    RequestStatus = ReadStatus::NoSignalStart;
  else if (!IsEnded)
    RequestStatus = ReadStatus::NoSignalEnd;
  else
  {
    EchoDelayMcr = Duration;
    RequestStatus = ReadStatus::Success;
  }
}

void SR04::Throw()
{
  const uint32_t
    StrobeDurationMcr = 250;

  pinMode(TriggerPin, OUTPUT);
  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(StrobeDurationMcr);
  digitalWrite(TriggerPin, LOW);
}

void SR04::Catch(bool *IsStarted, bool *IsEnded, uint32_t *Duration)
{
  *IsStarted = false;
  *IsEnded = false;
  *Duration = 0;

  const uint32_t
    StartTimeoutMcr = 2500,
    DurationTimeoutMcr = 250000;

  pinMode(EchoPin, INPUT);

  uint32_t StartMcr = me_SignalTime::GetLevelTime(EchoPin, LOW, StartTimeoutMcr);
  if (StartMcr != 0)
  {
    *IsStarted = true;
    uint32_t DurationMcr = me_SignalTime::GetLevelTime(EchoPin, HIGH, DurationTimeoutMcr);
    if (DurationMcr != 0)
    {
      *IsEnded = true;
      *Duration = DurationMcr;
    }
  }
}
