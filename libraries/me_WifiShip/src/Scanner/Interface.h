// WifiShip scanner

/*
  Status: stable
  Version: 2
  Last mod.: 2024-02-20
*/

/*
  Design

    Scanner
      // Optional parameters
      IncludeHidden: bool

      // Action
      Scan(): bool

      // Result
      Stations: array[] of Station
      GetStations_Length(): uint

    Station
      Id == Common_CraftIdentity.CraftId // MAC
      Name == Common_CraftIdentity.CraftName // SSID
      Channel:
        Strength: int // RSSI
        Band: uint // channel number
        Security:
          * WEP - cracked
          * WPA 1 - deprecated
          * WPA 1 + WPA 2 - deprecated support
          * WPA 2 - ok
*/

#pragma once

#include "Common/CraftIdentity/Interface.h"

namespace me_WifiShip_Scanner
{
  // Station id (MAC)
  typedef me_WifiShip_Common_CraftIdentity::TCraftId TStationId;

  // Station name (SSID)
  typedef me_WifiShip_Common_CraftIdentity::TCraftName TStationName;

  // Security
  enum struct TSecurityProtocol
  {
    None,
    Wep,
    Wpa,
    Wpa2,
    AnyWpa, // WPA 1 + WPA 2
    Unknown
  };

  // Channel
  struct TChannel
  {
    TSint_1 Strength; // RSSI
    TUint_1 Band; // channel number
    TSecurityProtocol Security;
  };

  // Station
  struct TStation
  {
    TStationId Id;
    TStationName Name;
    TBool IsHidden;
    TChannel Channel;
  };

  // Scanner
  class TScanner
  {
    public:
      TBool Init();

      TBool IncludeHidden;
      TBool Scan();
      TUint_1 GetStations_Length();
      TStation* Stations;

    private:
      TUint_1 Stations_Length;
      void FillStationInfo(TStation* Station, TUint_1 ScanIndex);
      TSecurityProtocol MapSecurityProtocol(TUint_1 Inner_SecurityProtocol);
  };
}

/*
  2024-01-16
  2024-02-07
*/
