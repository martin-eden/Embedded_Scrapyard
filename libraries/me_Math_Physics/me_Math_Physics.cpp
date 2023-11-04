// Functions to calculate sound properties.

/*
  Status: stable
  Last mod.: 2023-11-04
  Version: 1
*/

#include "me_Math_Physics.h"

/*
    GetDistanceFromEcho_Cm(EchoDelay_Us -> ui4) -> f4

      Calculate distance from echo delay.

      Distance is float in centimeters. Delay is ui4 in microseconds.

      Assumed signal medium is air at 23 deg C.

        https://en.wikipedia.org/wiki/Speed_of_sound

      As it explained there, the atmospheric pressure is not important,
      only temperature is.

      I've interpolated values for 25 and 20 deg C (343.21, 346.13) to
      get value for 23 deg C. (Which is my room temperature.)
*/
float GetDistanceFromEcho_Cm(uint32_t EchoDelay_Us)
{
  const float SoundSpeed_MpS = 344.37;

  return ((EchoDelay_Us / 2) * 1e-6) * SoundSpeed_MpS * 1e2;
}

/*
  Convert meters per second value to standard gravity unit.

  https://en.wikipedia.org/wiki/Standard_gravity

  I just hate SI sometimes. 3G acceleration in Kerbal Space Program
  tells me a lot more than 29.41 m/s. labylame <3
*/
float MpsToG(float MpS)
{
  const float StandardGravity = 9.80665;
  return MpS / StandardGravity;
}

/*
  2022-11-01
  2023-11-04
*/
