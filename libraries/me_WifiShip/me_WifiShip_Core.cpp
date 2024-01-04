// WifiShip core implementation.

#include "me_WifiShip.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip_Core;

// --( Init )--
TBool TWifiShip_Core::Init()
{
  return true;
}

// --( Get/set Id (MAC) )--

TBool TWifiShip_Core::GetShipId(TCraftId* ShipId)
{
  bool Inner_Result;
  const uint8 Inner_Role = 0;
  const TUint_1 Inner_Mac_Size = 6;
  uint8 Inner_Mac[Inner_Mac_Size];

  Inner_Result = wifi_get_macaddr(Inner_Role, Inner_Mac);

  if (!Inner_Result)
  {
    return false;
  }

  memcpy(ShipId, Inner_Mac, TCraftId_Size);

  return true;
}

TBool TWifiShip_Core::SetShipId(TCraftId ShipId)
{
  bool Inner_Result;
  const uint8 Inner_Role = 0;
  const TUint_1 Inner_Mac_Size = 6;
  uint8 Inner_Mac[Inner_Mac_Size];

  memcpy(Inner_Mac, ShipId, Inner_Mac_Size);

  Inner_Result = wifi_set_macaddr(Inner_Role, Inner_Mac);

  if (!Inner_Result)
  {
    Serial.println("@SetShipId: Inner_Result is false.");
    return false;
  }

  return true;
}

// --( Get/set Name (SSID) )--

TBool TWifiShip_Core::GetShipName(TCraftName* ShipName)
{
  strncpy(ShipName[0], wifi_station_get_hostname(), TCraftName_Size);

  return true;
}

TBool TWifiShip_Core::SetShipName(TCraftName ShipName)
{
  TUint_1 Inner_Hostname_Size = 32;
  char Inner_Hostname[Inner_Hostname_Size];
  bool Inner_Result;

  strncpy(Inner_Hostname, ShipName, Inner_Hostname_Size);

  Inner_Result = wifi_station_set_hostname(Inner_Hostname);

  if (!Inner_Result)
  {
    return false;
  }

  return true;
}

// --( State import/export )--

TBool TWifiShip_Core::GetModuleState(TModuleState* CoreState)
{
  return
    GetShipId(&CoreState->Id) &&
    GetShipName(&CoreState->Name);
}

TBool TWifiShip_Core::SetModuleState(TModuleState CoreState)
{
  return
    SetShipId(CoreState.Id) &&
    SetShipName(CoreState.Name);
}

// --

/*
  2024-01-01
  2024-01-03
*/