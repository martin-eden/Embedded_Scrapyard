// <me_WifiShip_Ui.h> library test/demo.

/*
  Status: good base
  Version: 3
  Last mod.: 2024-01-01
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_WifiShip.h>

#include <me_WifiShip_Ui.h>

#include <me_Types.h>

const TUint_4
  SerialSpeed = 115200;

const TUint_4
  RescanInterval_S = 1.5 * 60;

const TUint_2 Message_MaxLength = 2 * 256;
TChar Message [Message_MaxLength];

me_WifiShip::TWifiShip WifiShip;

void setup()
{
  const TUint_2 SerialSetupDelay_Ms = 500;

  Serial.begin(SerialSpeed);

  delay(SerialSetupDelay_Ms);

  WifiShip.Init();

  Serial.println();
  Serial.println();

  PrintGreeting();
  PrintSettings();

  PrintShipIds();
  ReplaceShipIds();
}

void loop()
{
  // static TBool IsConnected = false;

  PrintShipIds();

  // WifiShip_Ui.PrintStations();

  // IsConnected = WifiShip_Ui.Connect();

  // if (IsConnected)
  // {
  //   Serial.println("Connected to station.");
  // }

  delay(RescanInterval_S * 1000);
}

void PrintGreeting()
{
  Serial.println("Demo of <me_WifiShip.h> and <me_WifiShip_Ui.h>.");
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
  me_WifiShip::TCraftIds ShipIds;

  if (WifiShip.GetShipIds(&ShipIds))
  {
    me_WifiShip_Ui::RepresentCraftIds(Message, Message_MaxLength, ShipIds);
    Serial.println(Message);
  }
}

void ReplaceShipIds()
{
  me_WifiShip::TCraftIds ShipIds;
  TBool SetIds;

  Serial.print("Trying to replace ship id's... ");

  ShipIds.Id[0] = 0xDE;
  ShipIds.Id[1] = 0xFA;
  ShipIds.Id[2] = 0xCE;
  ShipIds.Id[3] = 0xD0;
  ShipIds.Id[4] = 0x00;
  ShipIds.Id[5] = 0x00;

  strcpy(ShipIds.Name, "REBORN");

  SetIds = WifiShip.SetShipIds(ShipIds);

  if (SetIds)
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