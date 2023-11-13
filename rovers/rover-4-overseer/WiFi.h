// Setting-up WiFi

/*
  Version: 2
  Last mod.: 2023-11-13
*/

#pragma once

#include <Arduino.h>

namespace me_WiFi
{
  /*
    Terminology.

    Under "station" I assume what is usually called "Access Point".
    Under "station IP" I assume what is called "DNS IP".

    I just imagining docking process in Elite where we are ship docking
    to other structure (Station).

    Note "Wifi" in function names but "WiFi" in namespace.
  */

  // Initially we have name and id (MAC):
  String GetOurName();
  String GetOurMac_Str();

  // Then we are trying to dock with named station with password:
  bool SetupWifi(
    char const * StationName,
    char const * StationPassword,
    uint16_t Timeout_Ms = 32000
  );

  // Is we succeeded, Channel is established. Channel has strength (RSSI).
  int8_t GetRssi_Dbm();

  // Channel has two ends, station end and ship end.
  String GetStationIp_Str();
  String GetOurIp_Str();

  // After channel is established we can peek at station id:
  String GetStationMac_Str();
}

/*
  2023-11-07
  2023-11-13
*/
