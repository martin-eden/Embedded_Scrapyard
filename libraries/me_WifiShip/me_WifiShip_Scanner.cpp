// WifiShip scanner implementation.

#include "me_WifiShip_Scanner.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip_Scanner;

// --( Init )--
TBool TWifiShip_Scanner::Init()
{
  return true;
}

// --( Scan )--
/*
  Scan for nearby stations.

  This routine returns number of stations found in output parameter.
  To get details about each station use GetStationInfo() later.
*/
TBool TWifiShip_Scanner::Scan(TUint_1* NumStationsFound)
{
  /*
    WIP
  */

  *NumStationsFound = 0;

  return false;

  /*
  TSint_1 Inner_ScanResult;
  const TBool Inner_AsyncScan = false;
  const TBool Inner_IncludeHidden = true;

  Inner_ScanResult = WiFi.scanNetworks(Inner_AsyncScan, Inner_IncludeHidden);

  if (Inner_ScanResult >= 0)
  {
    *NumStationsFound = Inner_ScanResult;
  }

  return (Inner_ScanResult >= 0);
  */
}


TBool TWifiShip_Scanner::Scan_GetStationInfo(TUint_1 StationIndex, TScannedStation* Station)
{
  /*
    WIP
  */

  return false;
}
