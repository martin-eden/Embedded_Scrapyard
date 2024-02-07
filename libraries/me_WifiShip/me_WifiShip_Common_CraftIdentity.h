// Definition of Name and Id, they are used in all modules

/*
  Status: stable
  Version: 1
  Last mod.: 2024-02-06
*/

/*
  Data structures

    Id: array[6] of TUint_1 // MAC
    Name: array[32] of TChar // SSID
*/

#pragma once

#include <me_Types.h>

namespace me_WifiShip_Common_CraftIdentity
{
  // Id (MAC):
  const TUint_1 TCraftId_Size = 6;
  typedef TUint_1 TCraftId[TCraftId_Size];

  // Name (SSID):
  const TUint_1 TCraftName_Size = 32 + 1;
  typedef TChar TCraftName[TCraftName_Size];
}

/*
  2024-02-06
    Exported Id and Name from modules.

    I don't like to create one more level in file names (Common_CraftIdentity)
    but having same couple of definitions in three modules is more ugly.
*/
