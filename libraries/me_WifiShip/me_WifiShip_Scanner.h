// WifiShip scanner

/*
  Status: redesigned
  Version: 1
  Last mod.: 2024-01-16
*/

/*
  Design

    Scanner
      IncludeHidden: bool
      Scan(): bool
      Stations: array[] of Station
      GetStations_Length(): uint

    Station
      Id: array[] uint // MAC
      Name: array[] char // SSID
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

#include <me_Types.h>

namespace me_WifiShip_Scanner
{
  // Id (MAC)
  const TUint_1 TStationId_Size = 6;
  typedef TUint_1 TStationId[TStationId_Size];

  // Name (SSID)
  const TUint_1 TStationName_Size = 32 + 1;
  typedef TChar TStationName[TStationName_Size];

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
    TStationId Id; // MAC
    TStationName Name; // SSID
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
*/
