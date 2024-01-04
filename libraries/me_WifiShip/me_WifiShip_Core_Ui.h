// Text representation of WiFi ship core.

/*
  Status: stable
  Version: 1
  Last mod.: 2024-01-03
*/

#pragma once

#include <me_Types.h>

#include "me_WifiShip_Core.h"

namespace me_WifiShip_Core_Ui
{
  void RepresentShipId(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Core::TCraftId ShipId
  );

  void RepresentShipName(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Core::TCraftName ShipName
  );

  void RepresentCoreState(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Core::TModuleState CoreState
  );
}
