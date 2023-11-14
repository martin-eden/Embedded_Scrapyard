#include "me_Wifi.h"

#include <ESP8266WiFi.h>

// ---

using namespace me_Wifi;

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
  CommentStream->printf("Setting-up WiFi: [\n");

  CommentStream->printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n"
    "  With timeout (s): %u\n",
    GetOurName().c_str(),
    GetOurMac_Str().c_str(),
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
        "    Station:\n"
        "      MAC: %s\n"
        "    Channel:\n"
        "      RSSI (dBm): %d\n"
        "      Station IP: %s\n"
        "      Our IP: %s\n",
        GetStationMac_Str().c_str(),
        GetRssi_Dbm(),
        GetStationIp_Str().c_str(),
        GetOurIp_Str().c_str()
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

// ---
String me_Wifi::GetOurName()
{
  return WiFi.hostname();
}

String me_Wifi::GetOurMac_Str()
{
  return WiFi.macAddress();
}

int8_t me_Wifi::GetRssi_Dbm()
{
  return WiFi.RSSI();
}

String me_Wifi::GetStationMac_Str()
{
  return WiFi.BSSIDstr();
}

String me_Wifi::GetStationIp_Str()
{
  return WiFi.dnsIP().toString();
}

String me_Wifi::GetOurIp_Str()
{
  return WiFi.localIP().toString();
}

// ---
