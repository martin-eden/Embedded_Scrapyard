/*
  Implementation terminology

  -- gift to scanner
  Station(extends Craft) - router that we can try to connect

    IsHidden - has no name, usually not something that welcomes connects

    Channel - offered frequency band. See Channel.

  Channel - radio band channel that is used for data transmission

    Band - number of channel. Physically it describes range of radio
      frequencies (band).

    Strength - received signal strength indicator (RSSI).

      Weird negative integer number that represents logarithm
      of signal-to-noise-ratio.

      Maybe I'll to convert internal representation to something
      more useful.

    SecurityProtocol - WEP, WPA 1, WPA 2 or (WPA 1 + WPA 2).

        WEP - cracked
        WPA 1 - deprecated
        WPA 1 + WPA 2 - deprecated support
        WPA 2 - normal router should use this

    # Actually we can retrieve more channel details but I don't
    # need them for my projects.
*/

#pragma once

#include <me_Types.h>

namespace me_WifiShip_Scanner
{
  // Id:
  const TUint_1 TShipId_Size = 6;
  typedef TUint_1 TShipId[TShipId_Size];

  // Name:
  const TUint_1 TShipName_Size = 32 + 1;
  typedef TChar TShipName[TShipName_Size];

  // Id + Name:
  struct TShipIds
  {
    TShipId Id; // MAC
    TShipName Name; // SSID
  };

  enum struct TSecurityProtocol
  {
    None,
    Wep,
    Wpa,
    Wpa2,
    AnyWpa, // WPA 1 + WPA 2
    Unknown
  };

  /*
    Scanned channel info. We have a different set of fields for
    describing channel of docked station. So no TChannel.
  */
  struct TScannedChannel
  {
    TUint_1 Band; // Channel Number
    TSint_1 Strength; // RSSI
    TSecurityProtocol SecurityProtocol;
  };

  struct TScannedStation : TShipIds
  {
    TBool IsHidden;
    TScannedChannel Channel;
  };

  class TWifiShip_Scanner
  {
    public:
      TBool Init();

      TBool Scan(TUint_1* NumStationsFound);
      TBool Scan_GetStationInfo(TUint_1 StationIndex, TScannedStation* Station);

    private:
  };
}
