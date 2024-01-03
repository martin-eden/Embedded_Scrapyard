// WifiShip docker implementation.

#include "me_WifiShip_Docker.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip_Docker;

// --( Init )--
TBool TWifiShip_Docker::Init()
{
  DockingStatus = TDockingStatus::Undocked;

  {
    const TUint_1 DefaultDockingTimeout_S = 30;
    SetDockingTimeout_S(DefaultDockingTimeout_S);
  }

  return true;
}

// --( DockTo )--
TDockingStatus TWifiShip_Docker::DockTo(
  TStationName StationName,
  TStationPassword StationPassword
)
{
  // --( inner data )--
  TUint_1 Inner_Ssid_Size = 32;
  char Inner_Ssid[Inner_Ssid_Size];

  TUint_1 Inner_Passphrase_Size = 64;
  char Inner_Passphrase[Inner_Passphrase_Size];

  unsigned long Inner_TimeoutLength = DockingTimeout_S * 1000;

  wl_status_t Inner_Begin_Result;
  int8_t Inner_WaitForConnect_Result;

  // --( fill data )--
  strncpy(Inner_Ssid, StationName, Inner_Ssid_Size);

  strncpy(Inner_Passphrase, StationPassword, Inner_Passphrase_Size);

  // --( call )--
  Inner_Begin_Result = WiFi.begin(Inner_Ssid, Inner_Passphrase);
  Serial.printf("@DockTo.Inner_Begin_Result = %u\n", Inner_Begin_Result);

  Inner_WaitForConnect_Result = WiFi.waitForConnectResult(Inner_TimeoutLength);

  // --( handle result )--
  DockingStatus = MapInnerStatus(Inner_WaitForConnect_Result);

  return GetDockingStatus();
}

// --( Undock )--
void TWifiShip_Docker::Undock()
{
  if (DockingStatus == TDockingStatus::Docked)
  {
    const bool Inner_Wifioff = false;
    const bool Innner_EraseCredentials = true;
    bool Inner_Result;

    Inner_Result = WiFi.disconnect(Inner_Wifioff, Innner_EraseCredentials);

    if (!Inner_Result)
    {
      // Well, we have not much options here.
    }
  }

  DockingStatus = TDockingStatus::Undocked;
}

// --( Get ship address )--
TBool TWifiShip_Docker::GetShipAddress(TAddress* ShipAddress)
{
  IPAddress Inner_Result;

  if (DockingStatus != TDockingStatus::Docked)
    return false;

  Inner_Result = WiFi.localIP();

  *ShipAddress[0] = Inner_Result[0];
  *ShipAddress[1] = Inner_Result[1];
  *ShipAddress[2] = Inner_Result[2];
  *ShipAddress[3] = Inner_Result[3];

  return true;
}

// --( Get station address )--
TBool TWifiShip_Docker::GetStationAddress(TAddress* StationAddress)
{
  IPAddress Inner_Result;

  if (DockingStatus != TDockingStatus::Docked)
    return false;

  Inner_Result = WiFi.dnsIP();

  *StationAddress[0] = Inner_Result[0];
  *StationAddress[1] = Inner_Result[1];
  *StationAddress[2] = Inner_Result[2];
  *StationAddress[3] = Inner_Result[3];

  return true;
}

// --( State management )--

TDockingStatus TWifiShip_Docker::GetDockingStatus()
{
  return DockingStatus;
}

void TWifiShip_Docker::SetDockingTimeout_S(TUint_1 aDockingTimeout_S)
{
  DockingTimeout_S = aDockingTimeout_S;
}

TUint_1 TWifiShip_Docker::GetDockingTimeout_S()
{
  return DockingTimeout_S;
}

// -- Implementation details

// --( Map inner result )--
TDockingStatus TWifiShip_Docker::MapInnerStatus(TSint_1 aInnerStatus)
{
  // aInnerStatus - sint from WiFi.waitForConnectResult()

  /*
    Those guys are returning sint(-1) when timeout and returning uint(status)
    for other cases. Disgusting.
  */
  if (aInnerStatus == -1)
  {
    return TDockingStatus::Nah_TimeoutReached;
  }

  wl_status_t InnerStatus = static_cast<wl_status_t>(aInnerStatus);

  switch (InnerStatus)
  {
    case WL_IDLE_STATUS:
      return TDockingStatus::Undocked;

    case WL_DISCONNECTED:
      return TDockingStatus::Undocked;

    case WL_CONNECTED:
      return TDockingStatus::Docked;

    case WL_NO_SSID_AVAIL:
      return TDockingStatus::Nah_StationNotFound;

    case WL_WRONG_PASSWORD:
      return TDockingStatus::Nah_WrongPassword;

    case WL_CONNECT_FAILED:
      return TDockingStatus::Nah_Other;

    case WL_CONNECTION_LOST:
    case WL_SCAN_COMPLETED:
    case WL_NO_SHIELD:
    default:
      return TDockingStatus::Nah_Other;
  }
}

// --
