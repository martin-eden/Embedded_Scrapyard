#include "WiFi.h"

#include <ESP8266WiFi.h>

// ---

using namespace me_WiFi;

String GetOurName();
String GetOurMac_Str();
int8_t GetRssi_Dbm();
String GetStationMac_Str();
String GetStationIp_Str();
String GetOurIp_Str();

/*
  Core function.

  Try to connect to given station with given password in given timeout.
*/
bool me_WiFi::SetupWifi(
  char const * StationName,
  char const * StationPassword,
  uint16_t Timeout_Ms
)
{
  Serial.printf("Setting-up WiFi: [\n");

  Serial.printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n"
    "  With timeout (ms): %u\n",
    GetOurName().c_str(),
    GetOurMac_Str().c_str(),
    StationName,
    Timeout_Ms
  );

  uint32_t StartTimeMs = millis();

  WiFi.begin(StationName, StationPassword);

  int8_t ConnectionStatus = WiFi.waitForConnectResult(Timeout_Ms);

  uint32_t FinishTimeMs = millis();
  uint32_t TimePassedMs = FinishTimeMs - StartTimeMs;

  switch (ConnectionStatus)
  {
    case WL_CONNECTED:
      Serial.printf(
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
      Serial.printf("  Can't see station.\n");
      break;

    case WL_WRONG_PASSWORD:
      Serial.printf("  Wrong password.\n");
      break;

    case WL_CONNECT_FAILED:
      Serial.printf("  I see station but connection failed. Probably wrong password.\n");
      break;

    case NA_STATE:
      Serial.printf("  We finished too early. Connection was not established.\n");
      break;

    default:
      Serial.printf("  [debug] Uncovered case %d.\n", ConnectionStatus);
      break;
  }

  Serial.printf("  Time taken (ms): %u\n", TimePassedMs);

  Serial.printf("]\n\n");

  return (ConnectionStatus == WL_CONNECTED);
}

// ---
String me_WiFi::GetOurName()
{
  return WiFi.hostname();
}

String me_WiFi::GetOurMac_Str()
{
  return WiFi.macAddress();
}

int8_t me_WiFi::GetRssi_Dbm()
{
  return WiFi.RSSI();
}

String me_WiFi::GetStationMac_Str()
{
  return WiFi.BSSIDstr();
}

String me_WiFi::GetStationIp_Str()
{
  return WiFi.dnsIP().toString();
}

String me_WiFi::GetOurIp_Str()
{
  return WiFi.localIP().toString();
}

// ---
