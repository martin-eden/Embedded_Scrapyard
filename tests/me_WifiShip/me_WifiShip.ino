// <me_WifiShip.h> test/demo.

/*
  Status: good base
  Version: 5
  Last mod.: 2024-02-07
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_WifiShip.h>

#include <me_WifiShip_Core_Ui.h>
#include <me_WifiShip_Docker_Ui.h>
#include <me_WifiShip_Scanner_Ui.h>

#include <me_Types.h>

// Our WifiShip:
me_WifiShip::TWifiShip WifiShip;

// Serial speed (baud):
const TUint_4 SerialSpeed = 115200;

// Main loop interval (s):
const TUint_4 MainLoopInterval_S = 1.5 * 60;

// Just a generic buffer for text output:
const TUint_2 Message_MaxLength = 4 * 256;
TChar Message [Message_MaxLength];

void setup()
{
  {
    const TUint_2 SerialSetupDelay_Ms = 5000;

    Serial.begin(SerialSpeed);
    delay(SerialSetupDelay_Ms);
    Serial.printf("\n\n");
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
  TestScanner();
  TestDocker();
}

void loop()
{
  delay(MainLoopInterval_S * 1000);

  TestScanner();
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
      "We are getting ship's id and name, changing em and getting again:\n"
      "\n"
    )
  );

  PrintCoreState();

  ChangeShipId();
  ChangeShipName();

  Serial.println();

  PrintCoreState();

  Serial.printf(
    PSTR(
      "----------------------( Core test done )-------------------------\n"
    )
  );
}

void PrintCoreState()
{
  RepresentCore(Message, Message_MaxLength, WifiShip.Core);
  Serial.print(Message);
}

void RepresentCore(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Core::TWifiShip_Core Core
)
{
  me_WifiShip_Core::TShipId ShipId;
  me_WifiShip_Core::TShipName ShipName;

  // Get data
  {
    Core.GetShipId(&ShipId);
    Core.GetShipName(&ShipName);
  }

  TUint_1 IdStr_MaxLength = 20;
  TChar IdStr[IdStr_MaxLength];

  TUint_1 NameStr_MaxLength = 40;
  TChar NameStr[NameStr_MaxLength];

  // Serialize parts
  {
    using namespace me_WifiShip_Core_Ui;

    RepresentShipId(IdStr, IdStr_MaxLength, ShipId);
    RepresentShipName(NameStr, NameStr_MaxLength, ShipName);
  }

  snprintf(
    Message,
    Message_MaxLength,
    PSTR(
      "Id: %s\n"
      "Name: %s\n"
    ),
    IdStr,
    NameStr
  );
}

TBool ChangeShipId()
{
  me_WifiShip_Core::TShipId ShipId;
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
  me_WifiShip_Core::TShipName ShipName;
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

TSint_1 CompareInts(TSint_1 A, TSint_1 B)
{
  if (A < B)
    return -1;
  else if (A > B)
    return 1;
  else
    return 0;
}

int StationsComparator(
  const void* A,
  const void* B
)
{
  me_WifiShip_Scanner::TStation StationA;
  me_WifiShip_Scanner::TStation StationB;

  StationA = *(const me_WifiShip_Scanner::TStation*) A;
  StationB = *(const me_WifiShip_Scanner::TStation*) B;

  return
    -CompareInts(StationA.Channel.Strength, StationB.Channel.Strength);
}

void TestScanner()
{
  Serial.printf(
    PSTR(
      "\n"
      "----------------------( Scanner test )---------------------------\n"
      "We will scan ether for stations, sort them by signal strength and\n"
      "display results.\n"
      "\n"
    )
  );

  WifiShip.Scanner.IncludeHidden = true;

  TBool ScanResult;

  ScanResult = WifiShip.Scanner.Scan();

  if (!ScanResult)
    Serial.println("Scan failed.");
  else
  {
    // --( Sort scan result )--
    qsort(
      WifiShip.Scanner.Stations,
      WifiShip.Scanner.GetStations_Length(),
      sizeof(me_WifiShip_Scanner::TStation),
      StationsComparator
    );

    me_WifiShip_Scanner_Ui::RepresentScanResult(
      Message,
      Message_MaxLength,
      WifiShip.Scanner.Stations,
      WifiShip.Scanner.GetStations_Length()
    );

    Serial.print(Message);
  }

  Serial.printf(
    PSTR(
      "----------------------( Scanner test done )----------------------\n"
    )
  );
}

void TestDocker()
{
  Serial.printf(
    PSTR(
      "\n"
      "----------------------( Docker test )------------------------------\n"
      "Test of docker module.\n"
      "\n"
      "We are trying to connect to preset station with preset password.\n"
      "In case of success, we will print ship address and station address.\n"
      "We will disconnect afterwards.\n"
      "\n"
    )
  );

  me_WifiShip_Docker::TStationName StationName = "YOUR ROUTER NAME";
  me_WifiShip_Docker::TStationPassword StationPassword = "YOUR ROUTER PASSWORD";
  me_WifiShip_Docker::TStatus DockingStatus;

  Serial.printf(
    (
      "Station name: %s\n"
      "Station password: %s\n"
    ),
    StationName,
    StationPassword
  );

  DockingStatus = WifiShip.Docker.DockTo(StationName, StationPassword);

  me_WifiShip_Docker_Ui::RepresentStatus(Message, Message_MaxLength, DockingStatus);
  Serial.printf("Status: %s\n", Message);

  {
    me_WifiShip_Docker::TAddress ShipAddress;
    TBool IsOk;

    IsOk = WifiShip.Docker.GetShipAddress(ShipAddress);

    if (IsOk)
    {
      me_WifiShip_Docker_Ui::RepresentAddress(Message, Message_MaxLength, ShipAddress);
      Serial.printf("Ship address: %s\n", Message);
    }
  }

  {
    me_WifiShip_Docker::TAddress StationAddress;
    TBool IsOk;

    IsOk = WifiShip.Docker.GetStationAddress(StationAddress);

    if (IsOk)
    {
      me_WifiShip_Docker_Ui::RepresentAddress(Message, Message_MaxLength, StationAddress);
      Serial.printf("Station address: %s\n", Message);
    }
  }

  WifiShip.Docker.Undock();

  DockingStatus = WifiShip.Docker.GetStatus();

  me_WifiShip_Docker_Ui::RepresentStatus(Message, Message_MaxLength, DockingStatus);
  Serial.printf("Status: %s\n", Message);

  Serial.printf(
    PSTR(
      "----------------------( Docker test done )-------------------------\n"
    )
  );
}

/*
  2023-12-26
  2023-12-28
  2023-12-30
  2023-12-31
  2024-01-01
  2024-01-04
  2024-01-13
  2024-01-16
  2024-01-22
  2024-02-03
  2024-02-07
*/
