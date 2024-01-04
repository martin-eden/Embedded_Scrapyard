// Text representation of WiFi ship core. Implementation.

/*
  Status: stable
  Version: 2
  Last mod.: 2024-01-04
*/

#include "me_WifiShip_Core_Ui.h"

#include <cstdio> // for snprintf()

// --( Represent Id (MAC) )--
void me_WifiShip_Core_Ui::RepresentShipId(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TCraftId ShipId
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%02X%02X%02X%02X%02X%02X",
    ShipId[0],
    ShipId[1],
    ShipId[2],
    ShipId[3],
    ShipId[4],
    ShipId[5]
  );
};

// --( Represent Name (SSID) )--
void me_WifiShip_Core_Ui::RepresentShipName(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TCraftName ShipName
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%s",
    ShipName
  );
};


// --( Represent Id + Name )--
void me_WifiShip_Core_Ui::RepresentCoreState(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TModuleState CoreState
)
{
  static const TChar MessageFmt[] PROGMEM =
    "+------+\n"
    "| Ship |\n"
    "===================================================\n"
    "| Id           | Name                             |\n"
    "|--------------+----------------------------------|\n"
    "| %s | %-32s |\n"
    "===================================================\n"
    ;

  const TUint_1 ShipIdStr_Size = 1 * 16;
  TChar ShipIdStr[ShipIdStr_Size];

  const TUint_1 ShipNameStr_Size = 3 * 16;
  TChar ShipNameStr[ShipNameStr_Size];

  RepresentShipId(ShipIdStr, ShipIdStr_Size, CoreState.Id);
  RepresentShipName(ShipNameStr, ShipNameStr_Size, CoreState.Name);

  snprintf(
    Message,
    Message_MaxLength,
    MessageFmt,
    ShipIdStr,
    ShipNameStr
  );
}

/*
  2023-12-31 The only release in 2023.
  2024-01-01
  2024-01-03
*/
