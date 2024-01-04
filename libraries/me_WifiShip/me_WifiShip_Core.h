// WifiShip core.

/*
  Status: good
  Version: 7
  Last mod.: 2024-01-03
*/

/*
  This module provides functionality to get/set ship ids.

  Id's are Name (SSID) and Id (MAC).

  Design

    Init(): bool

    --( Id )--
    GetShipId(): bool, Id
    SetShipId(Id): bool

    --( Name )--
    GetShipName(): bool, Name
    SetShipName(Name): bool

    --( State import/export )--
    GetModuleState(): bool, Id, Name
    SetModuleState(Id, Name): bool
*/

#pragma once

#include <me_Types.h>

namespace me_WifiShip_Core
{
  // Id (MAC):
  const TUint_1 TCraftId_Size = 6;
  typedef TUint_1 TCraftId[TCraftId_Size];

  // Name (SSID):
  const TUint_1 TCraftName_Size = 32 + 1;
  typedef TChar TCraftName[TCraftName_Size];

  // State:
  struct TModuleState
  {
    TCraftId Id;
    TCraftName Name;
  };

  // Core module:
  class TWifiShip_Core
  {
    public:
      TBool Init();

      TBool GetShipId(TCraftId* ShipId);
      TBool SetShipId(TCraftId ShipId);

      TBool GetShipName(TCraftName* ShipName);
      TBool SetShipName(TCraftName ShipName);

      TBool GetModuleState(TModuleState* CoreState);
      TBool SetModuleState(TModuleState CoreState);
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
