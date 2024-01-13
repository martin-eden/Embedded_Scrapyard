// WifiShip docker module (connect/disconnect to station).

/*
  Status: done
  Vesrion: 1
  Last mod.: 2024-01-14
*/

/*
  Design

    Types

      StationName: TChar[32] // SSID
      Address: TUint_1[4] // IP address
      Status:
        * Not docked
        * Docked
        * Failed:
          * Station not found
          * Wrong password
          * Some other shit

    Methods

      --( inherited )--
      Init(): bool

      --( Status )--
      GetStatus(): bool

      --( Docking )--
      DockTo(StationName, StationPassword): DockingStatus
      Undock() <-- post_assert(DockingStatus == Not docked)

      --( Channel )--
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
  enum struct TStatus
  {
    Undocked,
    Docked,
    Nah_StationNotFound,
    Nah_WrongPassword,
    Nah_TimeoutReached,
    Nah_Other
  };

  // Timeout
  const TUint_1 DefaultDockingTimeout_S = 30;

  class TWifiShip_Docker
  {
    public:
      TBool Init();

      // Docking
      TStatus DockTo(
        TStationName StationName,
        TStationPassword StationPassword
      );
      void Undock();

      // Status
      TStatus GetStatus();

      // Timeout
      TUint_1 GetDockingTimeout_S();
      void SetDockingTimeout_S(TUint_1 aDockingTimeout_S);
      // Address
      TBool GetShipAddress(TAddress ShipAddress);
      TBool GetStationAddress(TAddress StationAddress);

    private:
      TUint_1 DockingTimeout_S;
      TStatus Status;

      TStatus MapInnerStatus(TSint_1 aInnerStatus);
  };
}

/*
  2024-01-03
  2024-01-13
*/
