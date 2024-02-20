// WifiShip docker implementation

/*
  Status: done
  Version: 1
  Last mod.: 2024-02-20
*/

#include "Docker/Interface.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip_Docker;

// --( Init )--
TBool TDocker::Init()
{
  Status = TStatus::Undocked;

  SetDockingTimeout_S(DefaultDockingTimeout_S);

  return true;
}

// --( DockTo )--
TStatus TDocker::DockTo(
  TStationName StationName,
  TStationPassword StationPassword
)
{
  // --( Inner data )--
  const TUint_1 Inner_Ssid_Size = 32;
  char Inner_Ssid[Inner_Ssid_Size];

  const TUint_1 Inner_Passphrase_Size = 64;
  char Inner_Passphrase[Inner_Passphrase_Size];

  unsigned long Inner_TimeoutLength = GetDockingTimeout_S() * 1000;

  int8_t Inner_WaitForConnect_Result;

  // --( Fill data )--
  strncpy(Inner_Ssid, StationName, Inner_Ssid_Size);

  strncpy(Inner_Passphrase, StationPassword, Inner_Passphrase_Size);

  // --( Call )--
  WiFi.begin(Inner_Ssid, Inner_Passphrase);
  Inner_WaitForConnect_Result = WiFi.waitForConnectResult(Inner_TimeoutLength);

  // --( Handle result )--
  Status = MapInnerStatus(Inner_WaitForConnect_Result);

  return GetStatus();
}

// --( Undock )--
void TDocker::Undock()
{
  if (Status == TStatus::Docked)
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

  Status = TStatus::Undocked;
}

// --( Status )--
TStatus TDocker::GetStatus()
{
  return Status;
}

// --( Get station address )--
TBool TDocker::GetStationAddress(TAddress StationAddress)
{
  IPAddress Inner_Result;

  if (Status != TStatus::Docked)
    return false;

  Inner_Result = WiFi.gatewayIP();

  StationAddress[0] = Inner_Result[0];
  StationAddress[1] = Inner_Result[1];
  StationAddress[2] = Inner_Result[2];
  StationAddress[3] = Inner_Result[3];

  return true;
}

// --( Get ship address )--
TBool TDocker::GetShipAddress(TAddress ShipAddress)
{
  IPAddress Inner_Result;

  if (Status != TStatus::Docked)
    return false;

  Inner_Result = WiFi.localIP();

  ShipAddress[0] = Inner_Result[0];
  ShipAddress[1] = Inner_Result[1];
  ShipAddress[2] = Inner_Result[2];
  ShipAddress[3] = Inner_Result[3];

  return true;
}

// --( Timeout )--
TUint_1 TDocker::GetDockingTimeout_S()
{
  return DockingTimeout_S;
}

void TDocker::SetDockingTimeout_S(TUint_1 aDockingTimeout_S)
{
  DockingTimeout_S = aDockingTimeout_S;
}
// --()

// --( Map inner result )--
/*
  Convert result of WiFi.waitForConnectResult() to our status value.
*/
TStatus TDocker::MapInnerStatus(TSint_1 aInnerStatus)
{
  /*
    Those guys are returning sint(-1) when timeout and returning uint(status)
    for other cases. Disgusting.
  */
  if (aInnerStatus == -1)
  {
    return TStatus::Nah_TimeoutReached;
  }

  wl_status_t InnerStatus = static_cast<wl_status_t>(aInnerStatus);

  switch (InnerStatus)
  {
    case WL_IDLE_STATUS:
      return TStatus::Undocked;

    case WL_DISCONNECTED:
      return TStatus::Undocked;

    case WL_CONNECTED:
      return TStatus::Docked;

    case WL_NO_SSID_AVAIL:
      return TStatus::Nah_StationNotFound;

    case WL_WRONG_PASSWORD:
      return TStatus::Nah_WrongPassword;

    case WL_CONNECT_FAILED:
      return TStatus::Nah_Other;

    case WL_CONNECTION_LOST:
    case WL_SCAN_COMPLETED:
    case WL_NO_SHIELD:
    default:
      return TStatus::Nah_Other;
  }
}

// --

/*
  2024-01-03
  2024-01-13
  2024-01-14
*/
