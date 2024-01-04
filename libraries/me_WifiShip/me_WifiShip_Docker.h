// WifiShip docker module (connect/disconnect to station).

/*
  Status: in writing
  Vesrion: 0
  Last mod.: 2024-01-03
*/

/*
  Design

    Types

      StationName: SSID : TChar[32]
      Address: (IP address): TUint_1[4]
      DockingStatus:
        * Not docked
        * Docked
        * Failed:
          * Station not found
          * Wrong password
          * Some other shit

    Methods

      --( base )--
      Init(): bool

      --( docking )--
      DockTo(StationName, StationPassword): DockingStatus
      Undock() <-- post_assert(DockingStatus == Not docked)

      --( channel )--
      GetShipAddress(): bool, Address <-- local IP: 192.168.0.208
      GetStationAddress(): bool, Address <-- DNS IP: 192.168.0.1
*/

#pragma once

#include <me_Types.h>

namespace me_WifiShip_Docker
{
  // StationName (SSID)
  const TUint_1 TStationName_Size = 32 + 1;
  typedef TChar TStationName[TStationName_Size];

  // StationPassword (Password)
  const TUint_1 TStationPassword_Size = 64 + 1;
  typedef TChar TStationPassword[TStationPassword_Size];

  // Address (IP)
  const TUint_1 TAddress_Size = 4;
  typedef TUint_1 TAddress[TAddress_Size];

  // Docking status:
  enum struct TDockingStatus
  {
    Undocked,
    Docked,
    Nah_StationNotFound,
    Nah_WrongPassword,
    Nah_TimeoutReached,
    Nah_Other
  };

  class TWifiShip_Docker
  {
    public:
      TBool Init();

      TDockingStatus DockTo(
        TStationName StationName,
        TStationPassword StationPassword
      );
      void Undock();

      TUint_1 GetDockingTimeout_S();
      void SetDockingTimeout_S(TUint_1 aDockingTimeout_S);

      TBool GetShipAddress(TAddress* ShipAddress);
      TBool GetStationAddress(TAddress* StationAddress);

      TDockingStatus GetDockingStatus();

    private:
      TUint_1 DockingTimeout_S;
      TDockingStatus DockingStatus;

      TDockingStatus MapInnerStatus(TSint_1 aInnerStatus);
  };
}
