// WifiShip scanner implementation

/*
  Status: stable
  Version: 1
  Last mod.: 2024-02-20
*/

#include "Scanner/Interface.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip_Scanner;

// --( Init )--
TBool TScanner::Init()
{
  Stations_Length = 0;
  Stations = NULL;
  IncludeHidden = false;

  return true;
}

// --( Scan )--
/*
  Scan for nearby stations.

  This routine reallocates and fills <.Stations> array.
  To get length of array use GetStations_Length().
*/
TBool TScanner::Scan()
{
  // --( prepare )--
  int8_t Inner_ScanResult;
  const int8_t Inner_Async = false;
  const bool Inner_ShowHidden = IncludeHidden;

  // --( call )--
  Inner_ScanResult = WiFi.scanNetworks(Inner_Async, Inner_ShowHidden);

  // --( process )--
  TBool Result = (Inner_ScanResult >= 0);
  if (Result)
  {
    Stations_Length = Inner_ScanResult;

    // --( memory allocation )--
    if (Stations != NULL)
      delete[] Stations;

    Stations = new TStation[Stations_Length];
    // --

    for (TUint_1 ScanIndex = 0; ScanIndex < Stations_Length; ++ScanIndex)
    {
      FillStationInfo(&Stations[ScanIndex], ScanIndex);
    }
  }

  return Result;
}

TUint_1 TScanner::GetStations_Length()
{
  return Stations_Length;
}

/*
  Fill details about station.

  Station is referenced by index. Index is zero-based, limited by number
  of stations discovered by scan.
*/
void TScanner::FillStationInfo(TStation* Station, TUint_1 ScanIndex)
{
  // --( prepare )--
  bool Inner_Result;
  String Inner_StationName;
  uint8_t Inner_Cipher;
  int32_t Inner_Rssi;
  uint8_t* Inner_Id;
  int32_t Inner_BandNumber;
  bool Inner_IsHidden;

  // --( call )--
  Inner_Result = WiFi.getNetworkInfo(
    ScanIndex,
    Inner_StationName,
    Inner_Cipher,
    Inner_Rssi,
    Inner_Id,
    Inner_BandNumber,
    Inner_IsHidden
  );

  // --( process )--
  if (!Inner_Result)
  {
    Serial.println("@ FillStationInfo(): bad inner result. Early exit.");
    return;
  }

  // Station.Id
  memcpy(Station->Id, Inner_Id, sizeof(TStationId));

  // Station.Name
  strncpy(Station->Name, Inner_StationName.c_str(), sizeof(TStationName));

  // Station.IsHidden
  Station->IsHidden = Inner_IsHidden;

  // Station.Channel.Strength
  Station->Channel.Strength = Inner_Rssi;

  // Station.Channel.BandNumber
  Station->Channel.Band = Inner_BandNumber;

  // Station.Channel.Security
  Station->Channel.Security = MapSecurityProtocol(Inner_Cipher);
}

TSecurityProtocol TScanner::MapSecurityProtocol(TUint_1 Inner_SecurityProtocol)
{
  switch (Inner_SecurityProtocol)
  {
    case ENC_TYPE_NONE:
      return TSecurityProtocol::None;

    case ENC_TYPE_WEP:
      return TSecurityProtocol::Wep;

    case ENC_TYPE_TKIP:
      return TSecurityProtocol::Wpa;

    case ENC_TYPE_CCMP:
      return TSecurityProtocol::Wpa2;

    case ENC_TYPE_AUTO:
      return TSecurityProtocol::AnyWpa;

    case 255: // They are returning -1 in uint8_t function in unhandled cases.
      return TSecurityProtocol::Unknown;

    default:
      Serial.printf("@ Unexpected security protocol value: [%u]. Early exit.\n", Inner_SecurityProtocol);
      return TSecurityProtocol::Unknown;
  }
}

/*
  2024-01-16
  2024-01-22
  2024-02-07
*/
