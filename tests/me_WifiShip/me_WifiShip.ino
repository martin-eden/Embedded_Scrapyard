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

// Our WifiShip:
me_WifiShip::TWifiShip WifiShip;

// Serial speed (baud):
const TUint_4 SerialSpeed = 115200;

// Main loop interval (s):
const TUint_4 MainLoopInterval_S = 1.5 * 60;

// Just a generic buffer for text output:
const TUint_2 Message_MaxLength = 2 * 256;
TChar Message [Message_MaxLength];

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
  delay(MainLoopInterval_S * 1000);

  PrintCoreState();
}

void PrintGreeting()
{
  Serial.println("Demo of <me_WifiShip.h>.");
  Serial.println();
}

void PrintSettings()
{
  Serial.printf("Main loop interval (s): %lu\n", MainLoopInterval_S);
}

void TestCore()
{
  Serial.printf(
    PSTR(
      "\n"
      "----------------------( Core test )------------------------------\n"
      "Test of core module.\n"
      "\n"
      "We are getting ship's id and name, changing em and getting again:\n"
      "\n"
    )
  );

  PrintCoreState();

  ChangeShipId();
  ChangeShipName();

  PrintCoreState();

  Serial.printf(
    PSTR(
      "----------------------( Core test done )-------------------------\n"
    )
  );

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
