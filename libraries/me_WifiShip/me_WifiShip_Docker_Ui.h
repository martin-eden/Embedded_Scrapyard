#pragma once

#include <me_Types.h>

#include "me_WifiShip_Docker.h"

namespace me_WifiShip_Docker_Ui
{
  void RepresentStatus(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Docker::TStatus Status
  );

  void RepresentAddress(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Docker::TAddress Address
  );
}
