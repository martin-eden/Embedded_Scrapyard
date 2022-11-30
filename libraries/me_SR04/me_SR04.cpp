// Ultrasonic distance sensor HC-SR04.

/*
  Last mod.: 2022-12-01
*/

/*
  Fields

    bool NoSignalStart();
      Technically it means that we didn't see <EchoPin> goes HIGH.

    bool NoSignalEnd();
      Technically it means that we saw <EchoPin> goes LOW before timeout.

    void Ping(uint32_t *echoDelayMcr);
      Actually ping. Takes time.

    bool IsStarted;
    bool IsEnded;

    void Throw();
    void Catch();

  Uses

    <me_SignalTime.h>
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

uint8_t Sonar::GetTriggerPin()
{
  return TriggerPin;
}

uint8_t Sonar::GetEchoPin()
{
  return EchoPin;
}

void Sonar::Ping()
{
  Throw();
  Catch();
}

bool Sonar::NoSignalStart()
{
  return !IsStarted;
}

bool Sonar::NoSignalEnd()
{
  return !IsEnded;
}

uint32_t Sonar::EchoDelayMcr()
{
  return _EchoDelayMcr;
}

void Sonar::Throw()
{
  pinMode(TriggerPin, OUTPUT);

  const uint32_t
    StrobeDurationMcr = 250;

  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(StrobeDurationMcr);
  digitalWrite(TriggerPin, LOW);
}

void Sonar::Catch()
{
  pinMode(EchoPin, INPUT);

  IsStarted = false;
  IsEnded = false;
  _EchoDelayMcr = 0;

  const uint32_t
    StartTimeoutMcr = 2500,
    DurationTimeoutMcr = 25000;

  uint32_t StartMcr = me_SignalTime::GetLevelTime(EchoPin, LOW, StartTimeoutMcr);
  if (StartMcr != 0)
  {
    IsStarted = true;
    uint32_t DurationMcr = me_SignalTime::GetLevelTime(EchoPin, HIGH, DurationTimeoutMcr);
    if (DurationMcr != 0)
    {
      IsEnded = true;
      _EchoDelayMcr = DurationMcr;
    }
  }
}
