// Ultrasonic distance sensor HC-SR04

/*
  Status: stable
  Version: 1.4
  Last mod.: 2022-11-13
*/

/*
  This sensor registers echo returned after emitting pulse signal.

  Unlike normal physics, to get distance we need to measure not delay
  between start of pulse and start of echo but the duration of signal
  on echo pin.

  After emitting strobe, there is some delay between(0.5, 2) ms
  (depending of flair of SR-04). And then echo pin goes HIGH. It stays
  high till start of receiving of echo. Or until hardware timeout which
  is like 200 ms. We are measuring duration of that HIGH state.

  Hardware timeout of 200 ms is like 34 m. Practically this sensor
  receives echo from distances less than 4 m. So we are using stricter
  timeout to filter noise echoes.
*/

#include "me_SR04.h"
#include <me_SignalTime.h>

using namespace me_SR04;

void SR04::Ping()
{
  RequestStatus = Status::Unknown;
  EchoDelayMcr = 0;

  bool IsStarted, IsEnded;

  Throw();
  Catch(&IsStarted, &IsEnded, &EchoDelayMcr);

  if (IsStarted && IsEnded)
    RequestStatus = Status::Success;
  else if (!IsStarted)
    RequestStatus = Status::NoSignalStart;
  else if (!IsEnded)
    RequestStatus = Status::NoSignalEnd;
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
    DurationTimeoutMcr = 25000;

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
