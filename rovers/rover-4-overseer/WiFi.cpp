#include "WiFi.h"

#include <ESP8266WiFi.h>

void SetupWiFi(char const * _StationName, char const * _StationPassword)
{
  Serial.println("Setting-up WiFi: [");

  Serial.printf(
    "  We are:\n"
    "    Name: %s\n"
    "    MAC: %s\n"
    "  Connecting to:\n"
    "    Station: %s\n",
    WiFi.hostname().c_str(),
    WiFi.macAddress().c_str(),
    _StationName
  );

  uint32_t StartTimeMs = millis();

  WiFi.begin(_StationName, _StationPassword);

  int8_t ConnectionStatus = WiFi.waitForConnectResult(32000);

  uint32_t FinishTimeMs = millis();
  uint32_t TimePassedMs = FinishTimeMs - StartTimeMs;

  switch (ConnectionStatus)
  {
    case WL_CONNECTED:
      Serial.println(
        "  Connected:"
      );
      Serial.println(
        "    Station:"
      );
      Serial.printf(
        "      RSSI (dBm): %d\n", WiFi.RSSI()
      );
      Serial.printf(
        "      MAC: %s\n", WiFi.BSSIDstr().c_str()
      );
      Serial.println(
        "    Our:"
      );
      Serial.print(
        "      IP: "
      );
      Serial.println(WiFi.localIP());
      Serial.print(
        "      DNS: "
      );
      Serial.println(WiFi.dnsIP());

      break;

    case WL_NO_SSID_AVAIL:
      Serial.println("  Can't see station.");
      break;

    case WL_WRONG_PASSWORD:
      Serial.println("  Wrong password.");
      break;

    case WL_CONNECT_FAILED:
      Serial.println("  I see station but connection failed. Probably wrong password.");
      break;

    case NA_STATE:
      Serial.println("  We finished too early. Connection has not came.");
      break;

    default:
      Serial.printf("  [debug] Uncovered case %d.\n", ConnectionStatus);
      break;
  }

  Serial.printf("  Time taken (ms): %u\n", TimePassedMs);

  Serial.println("]");
}

