// Last mod.: 2024-02-20

#pragma once

#include "Docker/Interface.h"

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
