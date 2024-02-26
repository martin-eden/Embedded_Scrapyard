// Setting-up WiFi

/*
  Version: 4
  Last mod.: 2024-02-26
*/

#pragma once

#include <Arduino.h>

namespace me_Wifi
{
  constexpr auto *CommentStream = &Serial;

  bool Init();

  // Dock with station by name and password:
  bool SetupWifi(
    char const * StationName,
    char const * StationPassword,
    uint16_t Timeout_S = 32
  );
}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-02-26
*/
