// Ultrasonic distance sensor HC-SR04

/*
  Status: forming
  Version: 1.0
  Last mod.: 2022-06-20
*/

/*
  This sensor registeres echo returned after emitting pulse signal.

  Unlike normal physics, to get distance we need to measure
  duration of echo, not delay between end of pulse and start of echo.

  In sensor circuit there is builtin echo duration timeout ~= 200ms.
*/

#include "me_SR04.h"
#include <me_SignalTime.h>

using namespace me_SR04;
using namespace me_SignalTime;

void me_SR04::SR04::Request()
{
  const uint32_t
    PulseDurationMcr = 10,
    EchoStartTimeoutMcr = 1000,
    EchoMaxDurationMcr = 180000;

  RequestStatus = ReadStatus::Unknown;

  pinMode(TriggerPin, OUTPUT);
  digitalWrite(TriggerPin, LOW);

  pinMode(EchoPin, INPUT);

  digitalWrite(TriggerPin, HIGH);
  delayMicroseconds(PulseDurationMcr);
  digitalWrite(TriggerPin, LOW);

  EchoDelayMcr = GetLevelTime(EchoPin, LOW, EchoStartTimeoutMcr);
  if (EchoDelayMcr == 0)
  {
    RequestStatus = ReadStatus::Error;
  }
  else
  {
    EchoDurationMcr = GetLevelTime(EchoPin, HIGH, EchoMaxDurationMcr);
    if (EchoDurationMcr == 0)
      RequestStatus = ReadStatus::Error;
    else
      RequestStatus = ReadStatus::Success;
  }
}
