#pragma once

#include <me_Types.h>

namespace me_Uart
{
  enum StandardSpeeds:TUint_4
  {
    Bps_300 = 300,
    Bps_600 = 600, // == prev * 2
    Bps_1200 = 1200, // ..
    Bps_2400 = 2400,
    Bps_4800 = 4800,
    Bps_9600 = 9600,
    Bps_19200 = 19200,
    Bps_38400 = 38400, // == prev * 2
    Bps_57600 = 57600, // ha-ha: == prev * 1.5
    Bps_115200 = 115200, // == prev * 2
    Bps_230400 = 230400, // ..
    Bps_460800 = 460800,
    Bps_921600 = 921600
  };

  const StandardSpeeds Arduino_Failsafe_Bps = Bps_9600;
  const StandardSpeeds Arduino_Normal_Bps = Bps_57600;
  const StandardSpeeds Esplora_Normal_Bps = Bps_115200;
}
