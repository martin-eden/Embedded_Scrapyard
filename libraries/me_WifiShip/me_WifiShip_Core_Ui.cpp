// Text representation of WiFi ship core. Implementation.

/*
  Status: stable
  Version: 3
  Last mod.: 2024-02-07
*/

#include "me_WifiShip_Core_Ui.h"

#include "me_WifiShip_Common_CraftIdentity_Ui.h"

void me_WifiShip_Core_Ui::RepresentShipId(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TShipId ShipId
)
{
  me_WifiShip_Common_CraftIdentity_Ui::RepresentCratfId(
    Message,
    Message_MaxLength,
    ShipId
  );
}

void me_WifiShip_Core_Ui::RepresentShipName(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TShipName ShipName
)
{
  me_WifiShip_Common_CraftIdentity_Ui::RepresentCraftName(
    Message,
    Message_MaxLength,
    ShipName
  );
}

/*
  2023-12-31 The only release in 2023.
  2024-01-01
  2024-01-03
  2024-02-07
*/
