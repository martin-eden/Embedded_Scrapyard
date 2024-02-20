// Representation of Name and Id. Implementation.

/*
  Status: stable
  Version: 1
  Last mod.: 2024-02-20
*/

#include "Common/CraftIdentity/Ui/Interface.h"

#include <cstdio> // snprintf()

// Represent Id (MAC)
void me_WifiShip_Common_CraftIdentity_Ui::RepresentCratfId(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Common_CraftIdentity::TCraftId CraftId
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%02X:%02X:%02X:%02X:%02X:%02X",
    CraftId[0],
    CraftId[1],
    CraftId[2],
    CraftId[3],
    CraftId[4],
    CraftId[5]
  );
}

// Represent Name (SSID)
void me_WifiShip_Common_CraftIdentity_Ui::RepresentCraftName(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Common_CraftIdentity::TCraftName CraftName
)
{
  snprintf(Message, Message_MaxLength, "%s", CraftName);
}

/*
  2024-02-06
    Exported Id and Name from modules.

    I don't like to create one more level in file names (Common_CraftIdentity)
    but having same couple of definitions in three modules is more ugly.
*/
