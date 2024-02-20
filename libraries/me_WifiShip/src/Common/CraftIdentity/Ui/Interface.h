// Representation of Name and Id

/*
  Status: stable
  Version: 1
  Last mod.: 2024-02-20
*/

/*
  Design

    RepresentCraftId
      (Str, StrLen, Id)

    RepresentCraftName
      (Str, StrLen, Name)
*/

#pragma once

#include "Common/CraftIdentity/Interface.h"

namespace me_WifiShip_Common_CraftIdentity_Ui
{
  void RepresentCratfId(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Common_CraftIdentity::TCraftId CraftId
  );

  void RepresentCraftName(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Common_CraftIdentity::TCraftName CraftName
  );
}

/*
  2024-02-06
    Exported Id and Name from modules.

    I don't like to create one more level in file names (Common_CraftIdentity)
    but having same couple of definitions in three modules is more ugly.
*/
