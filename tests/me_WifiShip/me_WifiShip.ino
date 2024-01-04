// <me_WifiShip.h> test/demo.

/*
  Status: good base
  Version: 4
  Last mod.: 2024-01-04
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
  // Network rescan interval (s):
  RescanInterval_S = 1.5 * 60;

// Just a generic char buffer for text output:
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

  TestCore();
}

void loop()
{
  delay(RescanInterval_S * 1000);

  PrintCoreState();
}

void PrintGreeting()
{
  Serial.println("Demo of <me_WifiShip.h>.");
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

void TestCore()
{
  PrintCoreState();

  ChangeShipId();
  ChangeShipName();

  PrintCoreState();
}

void PrintCoreState()
{
  me_WifiShip_Core::TModuleState CoreState;

  if (WifiShip.Core.GetModuleState(&CoreState))
  {
    me_WifiShip_Core_Ui::RepresentCoreState(Message, Message_MaxLength, CoreState);
    Serial.println(Message);
  }
}

TBool ChangeShipId()
{
  me_WifiShip_Core::TCraftId ShipId;
  TBool Result;

  // Get:
  Result = WifiShip.Core.GetShipId(&ShipId);
  if (Result)
  {
    // Change:
    ShipId[0] = 0xDE;
    ShipId[1] = 0xFA;
    ShipId[2] = 0xCE;
    ShipId[3] = 0xD0;
    ShipId[4] = 0x00;
    ShipId[5] = 0x00;
    // Set:
    Result = WifiShip.Core.SetShipId(ShipId);
  }

  return Result;
}

TBool ChangeShipName()
{
  me_WifiShip_Core::TCraftName ShipName;
  TBool Result;

  // Get:
  Result = WifiShip.Core.GetShipName(&ShipName);
  if (Result)
  {
    // Change:
    strcpy(ShipName, "REBORN");
    // Set:
    Result = WifiShip.Core.SetShipName(ShipName);
  }

  return Result;
}

/*
  2023-12-26
  2023-12-28
  2023-12-30
  2023-12-31
  2024-01-01
*/
