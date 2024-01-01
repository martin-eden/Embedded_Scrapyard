#include "me_WifiShip.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK
// #include <Arduino.h> // Serial for debug

using namespace me_WifiShip;

// -- Id

TBool TWifiShip::GetShipId(TCraftId* ShipId)
{
  bool Inner_Result;
  const uint8 Inner_Role = 0;
  uint8 Inner_Mac[6];

  Inner_Result = wifi_get_macaddr(Inner_Role, Inner_Mac);

  if (!Inner_Result)
  {
    return false;
  }

  memcpy(ShipId, Inner_Mac, TCraftId_Size);

  return true;
}

TBool TWifiShip::SetShipId(TCraftId ShipId)
{
  return false;

  /*
  bool Inner_Result;
  const uint8 Inner_Role = 0;
  const TUint_1 Inner_Mac_Size = 6;
  uint8 Inner_Mac[Inner_Mac_Size];

  memcpy(Inner_Mac, ShipId, Inner_Mac_Size);

  Inner_Result = wifi_set_macaddr(Inner_Role, Inner_Mac);

  if (!Inner_Result)
  {
    return false;
  }

  return true;
  */
}

// -- Name

TBool TWifiShip::GetShipName(TCraftName* ShipName)
{
  strncpy(ShipName[0], WiFi.hostname().c_str(), TCraftName_Size);

  return true;
}

TBool TWifiShip::SetShipName(TCraftName ShipName)
{
  return false;

  /*
  TUint_1 Inner_Hostname_Size = 32;
  char Inner_Hostname[Inner_Hostname_Size];
  bool Inner_Result;

  strncpy(Inner_Hostname, reinterpret_cast<const char*>(ShipName), Inner_Hostname_Size);

  Inner_Result = wifi_station_set_hostname(reinterpret_cast<const char*>(Inner_Hostname));

  if (!Inner_Result)
  {
    return false;
  }

  return true;
  */
}

// -- Id + Name

TBool TWifiShip::GetShipIds(TCraftIds* ShipIds)
{
  return
    GetShipId(&ShipIds->Id) &&
    GetShipName(&ShipIds->Name);
}

TBool TWifiShip::SetShipIds(TCraftIds ShipIds)
{
  return
    SetShipId(ShipIds.Id) &&
    SetShipName(ShipIds.Name);
}

// --

/*
  2024-01-01
*/
