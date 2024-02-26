// Setting up WiFi connection.

/*
  Version: 4
  Last mod.: 2024-02-26
*/

#include "me_Wifi.h"

#include <me_WifiShip.h>
#include <Core/Ui/Interface.h>
#include <Scanner/Ui/Interface.h>
#include <Docker/Ui/Interface.h>

#include <ESP8266WiFi.h>

// ---

int8_t GetRssi_Dbm()
{
  return WiFi.RSSI();
}

String GetStationMac_Str()
{
  return WiFi.BSSIDstr();
}

// ---

me_WifiShip::TWifiShip WifiShip;

using namespace me_Wifi;

bool me_Wifi::Init()
{
  return WifiShip.Init();
}

/*
  Core function.

  Try to connect to given station with given password in given timeout.
*/
bool me_Wifi::SetupWifi(
  char const * StationName,
  char const * StationPassword,
  uint16_t Timeout_S
)
{
  TUint_1 ShipIdStr_MaxLength = 20;
  TChar ShipIdStr[ShipIdStr_MaxLength];

  TUint_1 ShipNameStr_MaxLength = 40;
  TChar ShipNameStr[ShipNameStr_MaxLength];

  CommentStream->printf("Setting-up WiFi: [\n");

  {
    me_WifiShip_Core::TShipId ShipId;
    me_WifiShip_Core::TShipName ShipName;

    WifiShip.Core.GetShipId(&ShipId);
    WifiShip.Core.GetShipName(&ShipName);

    {
      using namespace me_WifiShip_Core_Ui;
      RepresentShipId(ShipIdStr, ShipIdStr_MaxLength, ShipId);
      RepresentShipName(ShipNameStr, ShipNameStr_MaxLength, ShipName);
    }
  }

  CommentStream->printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n"
    "    Password: %s\n"
    "  With timeout (s): %u\n",
    ShipNameStr,
    ShipIdStr,
    StationName,
    StationPassword,
    Timeout_S
  );

  uint32_t StartTimeMs = millis();

  WifiShip.Docker.SetDockingTimeout_S(Timeout_S);

  me_WifiShip_Docker::TStatus DockingStatus;

  DockingStatus = WifiShip.Docker.DockTo(StationName, StationPassword);

  uint32_t FinishTimeMs = millis();
  uint32_t TimePassedMs = FinishTimeMs - StartTimeMs;

  {
    TChar DockingStatus_Str[20];
    me_WifiShip_Docker_Ui::RepresentStatus(
      DockingStatus_Str,
      sizeof(DockingStatus_Str),
      DockingStatus
    );

    CommentStream->printf("Docking status: %s\n", DockingStatus_Str);
  }

  if (DockingStatus == me_WifiShip_Docker::TStatus::Docked)
  {
    me_WifiShip_Docker::TAddress ShipAddress;
    WifiShip.Docker.GetShipAddress(ShipAddress);

    TChar ShipAddress_Str[20];
    me_WifiShip_Docker_Ui::RepresentAddress(
      ShipAddress_Str,
      sizeof(ShipAddress_Str),
      ShipAddress
    );

    me_WifiShip_Docker::TAddress StationAddress;
    WifiShip.Docker.GetStationAddress(StationAddress);

    TChar StationAddress_Str[20];
    me_WifiShip_Docker_Ui::RepresentAddress(
      StationAddress_Str,
      sizeof(StationAddress_Str),
      StationAddress
    );

    CommentStream->printf(
      "    Station:\n"
      "      MAC: %s\n"
      "    Channel:\n"
      "      RSSI (dBm): %d\n"
      "      Station IP: %s\n"
      "      Our IP: %s\n",
      GetStationMac_Str().c_str(),
      GetRssi_Dbm(),
      StationAddress_Str,
      ShipAddress_Str
    );
  }

  CommentStream->printf("  Time taken (ms): %u\n", TimePassedMs);

  CommentStream->printf("]\n\n");

  return (DockingStatus == me_WifiShip_Docker::TStatus::Docked);
}

/*
  2023-11-13
  2023-11-14
  2024-02-26
*/
