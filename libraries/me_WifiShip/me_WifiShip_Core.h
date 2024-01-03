// WifiShip core.

/*
  Status: good
  Version: 7
  Last mod.: 2024-01-03
*/

/*
  This module provides functionality to get/set ship ids.

  Id's are name (SSID) and id (MAC).

  Design

    Init(): bool

    --( Id )--
    GetShipId(): bool, Id
    SetShipId(Id): bool

    --( Name )--
    GetShipName(): bool, Name
    SetShipName(Name): bool

    --( Id + Name )--
    GetShipIds(): Id, Name
    SetShipIds(Id, Name): bool

  Implementation structures

    ShipId: array[6] <-- MAC
    ShipName: array[32] <-- SSID
    ShipIds:
      Id: ShipId
      Name: ShipName
*/

#pragma once

#include <me_Types.h>

namespace me_WifiShip_Core
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

  // Core:
  class TWifiShip_Core
  {
    public:
      TBool Init();

      TBool GetShipId(TShipId* ShipId);
      TBool SetShipId(TShipId ShipId);

      TBool GetShipName(TShipName* ShipName);
      TBool SetShipName(TShipName ShipName);

      TBool GetShipIds(TShipIds* ShipIds);
      TBool SetShipIds(TShipIds ShipIds);
  };
}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-12-28 -- code structural design, network scan
  2023-12-31 -- using <me_Types.h>
  2024-01-01 -- splat to three modules: ship = (frame, scanner, docker)
  2024-01-03
*/
