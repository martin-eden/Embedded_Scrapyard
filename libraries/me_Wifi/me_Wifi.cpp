// Wi-Fi functions that I need

/*
  Status: reforming
  Version: 4
  Last mod.: 2023-12-30
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
bool me_Wifi::Scan(uint8_t* NumStationsFound)
{
  int8_t Inner_ScanResult;
  const bool Inner_AsyncScan = false;
  const bool Inner_IncludeHidden = true;

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
bool me_Wifi::GetStationInfo(
  uint8_t StationIndex,
  TStation* Station
)
{
  bool Inner_Result;
  String Inner_StationName;
  uint8_t Inner_Cipher;
  int32_t Inner_Rssi;
  uint8_t* Inner_Id;
  int32_t Inner_BandNumber;
  bool Inner_IsHidden;

  const uint8_t Station_Id_MaxLength = TNode_Settings::Id_MaxLength;
  char Station_Id [Station_Id_MaxLength];

  Inner_Result = WiFi.getNetworkInfo(
    StationIndex,
    Inner_StationName,
    Inner_Cipher,
    Inner_Rssi,
    Inner_Id,
    Inner_BandNumber,
    Inner_IsHidden
  );

  if (!Inner_Result)
  {
    CommentStream->println("@ GetStationInfo(): early exit.");

    return false;
  }

  // --

  // Station.Name
  strncpy(Station->Name, Inner_StationName.c_str(), TNode_Settings::Name_MaxLength);

  // Station.Id
  snprintf(
    Station_Id,
    Station_Id_MaxLength,
    "%02X:%02X:%02X:%02X:%02X:%02X",
    Inner_Id[0],
    Inner_Id[1],
    Inner_Id[2],
    Inner_Id[3],
    Inner_Id[4],
    Inner_Id[5]
  );
  strncpy(Station->Id, Station_Id, Station_Id_MaxLength);

  // Station.IsHidden
  Station->IsHidden = Inner_IsHidden;

  // --

  // Channel.BandNumber
  Station->Channel.BandNumber = Inner_BandNumber;

  // Channel.SignalStrength
  Station->Channel.SignalStrength = Inner_Rssi;

  // Channel.SecurityProtocol
  {
    TSecurityProtocol SecurityProtocol;
    switch (Inner_Cipher)
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

      case 255: // They are returning -1 in uint8_t function in unhandled cases.
        SecurityProtocol = TSecurityProtocol::Unknown;
        break;

      default:
        CommentStream->printf("@ Unexpected security settings value: [%u]. Early exit.\n", Inner_Cipher);
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
bool me_Wifi::ConnectTo(
  char const * StationName,
  char const * StationPassword,
  uint16_t Timeout_S
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

  uint32_t StartTimeMs = millis();

  WiFi.begin(StationName, StationPassword);

  uint16_t Timeout_Ms = 1000 * Timeout_S;
  int8_t ConnectionStatus = WiFi.waitForConnectResult(Timeout_Ms);

  uint32_t FinishTimeMs = millis();
  uint32_t TimePassedMs = FinishTimeMs - StartTimeMs;

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

  CommentStream->printf("  Time taken (ms): %u\n", TimePassedMs);

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

int8_t me_Wifi::GetDistance_Dbm()
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
*/
