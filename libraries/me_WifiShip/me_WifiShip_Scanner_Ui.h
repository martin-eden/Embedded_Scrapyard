// WifiShip scanner UI

#pragma once

#include <me_Types.h>

#include "me_WifiShip_Scanner.h"

namespace me_WifiShip_Scanner_Ui
{
  void RepresentScanResult(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Scanner::TStation* Stations,
    TUint_1 NumStations
  );
}
