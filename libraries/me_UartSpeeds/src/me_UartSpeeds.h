// Named UART speeds

/*
  Well, I don't like remembering 57600. Or 115200. Or 9600.

  So I prefer named constants:

    Arduino_Normal_Bps
    Esplora_Normal_Bps
    Arduino_Failsafe_Bps

  Of course in code you need

    #include <me_Uart.h>
    ...
    Serial.begin(me_Uart::Arduino_Normal_Bps);

  instead of

    Serial.begin(57600);

  Choice is yours, there are no policies, just possibilities.

  This module has nothing more so far.
*/

// Author: Martin Eden
// Last mod.: 2024-04-09

#pragma once

#include <me_Types.h>

namespace me_UartSpeeds
{
  enum StandardSpeeds:TUint_4
  {
    Bps_300 = 300,
    Bps_600 = 600, // == prev * 2
    Bps_1200 = 1200, // ditto
    Bps_2400 = 2400,
    Bps_4800 = 4800,
    Bps_9600 = 9600,
    Bps_19200 = 19200,
    Bps_38400 = 38400,
    Bps_57600 = 57600, // ha-ha: == prev * 1.5
    Bps_115200 = 115200, // == prev * 2
    Bps_230400 = 230400, // ditto
    Bps_460800 = 460800,
    Bps_921600 = 921600
  };

  const StandardSpeeds Arduino_Failsafe_Bps = Bps_9600;
  const StandardSpeeds Arduino_Normal_Bps = Bps_57600;
  const StandardSpeeds Esplora_Normal_Bps = Bps_115200;
}
