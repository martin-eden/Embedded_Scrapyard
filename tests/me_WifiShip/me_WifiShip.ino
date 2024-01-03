// <me_WifiShip.h> test/demo.

/*
  Status: good base
  Version: 4
  Last mod.: 2024-01-03
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_WifiShip.h>
#include <me_WifiShip_Core_Ui.h>

#include <me_Types.h>

const TUint_4
  SerialSpeed = 115200;

const TUint_4
  // Network rescan interval:
  RescanInterval_S = 1.5 * 60;

// Just generic char buffer for text output:
const TUint_2 Message_MaxLength = 2 * 256;
TChar Message [Message_MaxLength];

// Our WifiShip:
me_WifiShip::TWifiShip WifiShip;

void setup()
{
  {
    const TUint_2 SerialSetupDelay_Ms = 500;

    Serial.begin(SerialSpeed);
    delay(SerialSetupDelay_Ms);
    Serial.println();
    Serial.println();
  }

  PrintGreeting();

  PrintSettings();

  if (!WifiShip.Init())
  {
    Serial.println("@ Failed to init WifiShip. Early exit.");
    return;
  }

  PrintShipIds();
  ReplaceShipIds();
  PrintShipIds();
}

void loop()
{
  delay(RescanInterval_S * 1000);

  PrintShipIds();
}

void PrintGreeting()
{
  Serial.println("Demo of <me_WifiShip_Core.h> and <me_WifiShip_Core_Ui.h>.");
  Serial.println();
}

void PrintSettings()
{
  snprintf(
    Message,
    Message_MaxLength,
    "Rescan interval (s): %lu",
    RescanInterval_S
  );
  Serial.println(Message);
}

void PrintShipIds()
{
  me_WifiShip_Core::TShipIds ShipIds;

  if (WifiShip.Core.GetShipIds(&ShipIds))
  {
    me_WifiShip_Core_Ui::RepresentShipIds(Message, Message_MaxLength, ShipIds);
    Serial.println(Message);
  }
}

void ReplaceShipIds()
{
  me_WifiShip_Core::TShipIds ShipIds;
  TBool Inner_Result;

  Serial.print("Replacing ship id's... ");

  ShipIds.Id[0] = 0xDE;
  ShipIds.Id[1] = 0xFA;
  ShipIds.Id[2] = 0xCE;
  ShipIds.Id[3] = 0xD0;
  ShipIds.Id[4] = 0x00;
  ShipIds.Id[5] = 0x00;

  strcpy(ShipIds.Name, "REBORN");

  Inner_Result = WifiShip.Core.SetShipIds(ShipIds);

  if (Inner_Result)
    Serial.println("yep!");
  else
    Serial.println("nah.");
}

/*
  2023-12-26
  2023-12-28
  2023-12-30
  2023-12-31
  2024-01-01
*/
