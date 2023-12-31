// Wi-Fi functions that I need

/*
  Status: good except ConnectTo() output
  Version: 4
  Last mod.: 2023-12-31
*/

#include "me_Wifi.h"

#include <ESP8266WiFi.h>

// --

using namespace me_Wifi;

/*
  Scan for nearby stations.

  This routine returns number of stations found in output parameter.
  To get details about each station use GetStationInfo() later.
*/
TBool me_Wifi::Scan(TUint_1* NumStationsFound)
{
  TSint_1 Inner_ScanResult;
  const TBool Inner_AsyncScan = false;
  const TBool Inner_IncludeHidden = true;

  Inner_ScanResult = WiFi.scanNetworks(Inner_AsyncScan, Inner_IncludeHidden);

  if (Inner_ScanResult >= 0)
  {
    *NumStationsFound = Inner_ScanResult;
  }

  return (Inner_ScanResult >= 0);
}

/*
  Get deatils about station.

  Station is referecned by index. Index is zero-based, limited by number
  of stations nearby. So you should scan first to get the number of
  stations nearby.
*/
TBool me_Wifi::GetStationInfo(
  TUint_1 StationIndex,
  TStation* Station
)
{
  bool Inner_Result;
  String Inner_Ssid;
  uint8_t Inner_EncType;
  int32_t Inner_Rssi;
  uint8_t* Inner_Bssid;
  int32_t Inner_Channel;
  bool Inner_IsHidden;

  const TUint_1 Station_Id_MaxLength = TNode_Settings::Id_MaxLength;
  TChar Station_Id [Station_Id_MaxLength];

  Inner_Result =
    WiFi.getNetworkInfo(
      StationIndex,
      Inner_Ssid,
      Inner_EncType,
      Inner_Rssi,
      Inner_Bssid,
      Inner_Channel,
      Inner_IsHidden
    );

  if (!Inner_Result)
  {
    CommentStream->println("@ GetStationInfo(): early exit.");

    return false;
  }

  // --

  // Station.Name
  strncpy(Station->Name, Inner_Ssid.c_str(), TNode_Settings::Name_MaxLength);

  // Station.Id
  snprintf(
    Station_Id,
    Station_Id_MaxLength,
    "%02X:%02X:%02X:%02X:%02X:%02X",
    Inner_Bssid[0],
    Inner_Bssid[1],
    Inner_Bssid[2],
    Inner_Bssid[3],
    Inner_Bssid[4],
    Inner_Bssid[5]
  );
  strncpy(Station->Id, Station_Id, Station_Id_MaxLength);

  // Station.IsHidden
  Station->IsHidden = Inner_IsHidden;

  // --

  // Channel.BandNumber
  Station->Channel.BandNumber = Inner_Channel;

  // Channel.SignalStrength
  Station->Channel.SignalStrength = Inner_Rssi;

  // Channel.SecurityProtocol
  {
    TSecurityProtocol SecurityProtocol;
    switch (Inner_EncType)
    {
      case ENC_TYPE_NONE:
        SecurityProtocol = TSecurityProtocol::None;
        break;

      case ENC_TYPE_WEP:
        SecurityProtocol = TSecurityProtocol::Wep;
        break;

      case ENC_TYPE_TKIP:
        SecurityProtocol = TSecurityProtocol::Wpa;
        break;

      case ENC_TYPE_CCMP:
        SecurityProtocol = TSecurityProtocol::Wpa2;
        break;

      case ENC_TYPE_AUTO:
        SecurityProtocol = TSecurityProtocol::AnyWpa;
        break;

      case 255: // They are returning -1 in TUint_1 function in unhandled cases.
        SecurityProtocol = TSecurityProtocol::Unknown;
        break;

      default:
        CommentStream->printf("@ Unexpected security settings value: [%u]. Early exit.\n", Inner_EncType);
        return false;
    }

    Station->Channel.SecurityProtocol = SecurityProtocol;
  }

  return true;
}

/*
  Core function.

  Try to connect to given station with given password in given timeout.
*/
TBool me_Wifi::ConnectByName(
  TChar const * StationName,
  TChar const * StationPassword,
  TUint_1 Timeout_S
)
{
  CommentStream->printf("Setting-up WiFi: [\n");

  CommentStream->printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n"
    "  With timeout (s): %u\n",
    GetOurName().c_str(),
    GetOurId().c_str(),
    StationName,
    Timeout_S
  );

  TUint_4 StartTimeMs = millis();

  WiFi.begin(StationName, StationPassword);

  TUint_2 Timeout_Ms = 1000 * Timeout_S;
  Serial.printf("Timeout_Ms = %u\n", Timeout_Ms);

  TSint_1 ConnectionStatus = WiFi.waitForConnectResult(Timeout_Ms);

  TUint_4 FinishTimeMs = millis();
  TUint_4 TimePassedMs = FinishTimeMs - StartTimeMs;

  switch (ConnectionStatus)
  {
    case WL_CONNECTED:
      CommentStream->printf(
        "  Connected:\n"
        "    Channel:\n"
        "      RSSI (dBm): %d\n"
        "    Ship:\n"
        "      IP: %s\n"
        "    Station:\n"
        "      MAC: %s\n"
        "      IP: %s\n",
        GetDistance_Dbm(),
        GetOurAddress().c_str(),
        GetStationId().c_str(),
        GetStationAddress().c_str()
      );
      break;

    case WL_NO_SSID_AVAIL:
      CommentStream->printf("  Can't see station.\n");
      break;

    case WL_WRONG_PASSWORD:
      CommentStream->printf("  Wrong password.\n");
      break;

    case WL_CONNECT_FAILED:
      CommentStream->printf("  I see station but connection failed. Probably wrong password.\n");
      break;

    case NA_STATE:
      CommentStream->printf("  We finished too early. Connection was not established.\n");
      break;

    default:
      CommentStream->printf("  [debug] Uncovered case %d.\n", ConnectionStatus);
      break;
  }

  CommentStream->printf("  Time taken (ms): %lu\n", TimePassedMs);

  CommentStream->printf("]\n\n");

  return (ConnectionStatus == WL_CONNECTED);
}

// --

String me_Wifi::GetOurName()
{
  return WiFi.hostname();
}

String me_Wifi::GetOurId()
{
  return WiFi.macAddress();
}

String me_Wifi::GetOurAddress()
{
  return WiFi.localIP().toString();
}

TSint_1 me_Wifi::GetDistance_Dbm()
{
  return WiFi.RSSI();
}

String me_Wifi::GetStationId()
{
  return WiFi.BSSIDstr();
}

String me_Wifi::GetStationAddress()
{
  return WiFi.dnsIP().toString();
}

// --

/*
  2023-11-13
  2023-11-14
  2023-12-28
  2023-12-30
  2023-12-31
*/
