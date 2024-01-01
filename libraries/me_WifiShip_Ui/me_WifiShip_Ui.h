/*
  All that get-user-input and print-nice-table shit here.

  For core you want only <me_WifiShip.h>. But for debugging and
  base user interactions interface you want this, <me_WifiShip_Ui.h>.
*/

#pragma once

#include <me_WifiShip.h>
#include <me_Types.h>

namespace me_WifiShip_Ui
{
  using namespace me_WifiShip;

  void RepresentCraftId(
    TChar* Message,
    TUint_2 Message_MaxLength,
    TCraftId CraftId
  );

  void RepresentCraftName(
    TChar* Message,
    TUint_2 Message_MaxLength,
    TCraftName CraftName
  );

  void RepresentCraftIds(
    TChar* Message,
    TUint_2 Message_MaxLength,
    TCraftIds CraftIds
  );
}
