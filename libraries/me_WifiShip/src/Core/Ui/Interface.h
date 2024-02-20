// Text representation of WiFi ship core.

/*
  Status: stable
  Version: 2
  Last mod.: 2024-02-20
*/

#pragma once

#include "Core/Interface.h"

namespace me_WifiShip_Core_Ui
{
  void RepresentShipId(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Core::TShipId ShipId
  );

  void RepresentShipName(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Core::TShipName ShipName
  );
}

/*
  2024-01-03
  2024-02-07
*/
