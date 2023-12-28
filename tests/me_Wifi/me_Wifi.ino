// My WiFi utility library test/exploration.

/*
  Status: good base
  Version: 1
  Last mod.: 2023-12-28
*/

/*
  Board: Esplora (Wemos D1)
*/

#include <me_Wifi.h>

const
  uint32_t SerialSpeed = 115200;

const uint16_t Message_MaxLength = 500;
char Message[Message_MaxLength];

void setup()
{
  Serial.begin(SerialSpeed);

  delay(500);
  Serial.println();
  Serial.println();

  PrintGreeting();
}

void loop()
{
  PrintStatus();
  DoScan();
  delay(150 * 1000);
}

void PrintGreeting()
{
  Serial.println("Demo <me_Wifi.h>.");
  Serial.println();
}

void PrintStatus()
{
  static const char MessageFmt[] PROGMEM =
    "+====+\n"
    "| We |\n"
    "+==========================================+\n"
    "|          Name        |        MAC        |\n"
    "|----------------------+-------------------|\n"
    "| %-20s | %s |\n"
    "+==========================================+\n"
    ;

  {
    using namespace me_Wifi;

    snprintf(
      Message,
      Message_MaxLength,
      MessageFmt,
      GetOurName().c_str(),
      GetOurId().c_str()
    );
    Serial.println(Message);
  }

  // Serial.println(me_Wifi::GetStationIp_Str());

  // Serial.println(me_Wifi::GetOurIp_Str());

  // Serial.println(me_Wifi::GetStationMac_Str());
}

void DoScan()
{
  uint8_t NumStationsFound;
  bool ScanResult;

  Serial.printf("Doing scan... ");

  {
    using namespace me_Wifi;

    ScanResult = Scan(&NumStationsFound);
  }

  if (ScanResult)
    Serial.println("yep.");
  else
    Serial.println("nah!");

  if (ScanResult)
  {
    snprintf(Message, Message_MaxLength, "%u stations found.", NumStationsFound);
    Serial.println(Message);

    if (NumStationsFound > 0)
      PrintHeader();

    for (uint8_t StationIndex = 0; StationIndex < NumStationsFound; ++StationIndex)
    {
      using namespace me_Wifi;
      TStation StationInfo;
      TChannel OfferedChannel;

      GetStationInfo(StationIndex, &StationInfo, &OfferedChannel);

      PrintDetails(StationInfo, OfferedChannel);
    }

    if (NumStationsFound > 0)
      PrintFooter();
  }
}

void PrintHeader()
{
  snprintf(
    Message,
    Message_MaxLength,
    "+==========+                                    +==========+\n"
    "| Stations |                                    | Channels |\n"
    "+==============================================++======================================================+\n"
    "|        Name          |        MAC        | H || RSSI | Ch |               Security                   |\n"
    "|----------------------+-------------------+---||------+----+------------------------------------------|\n"
  );
  Serial.print(Message);
}

void PrintFooter()
{
  snprintf(
    Message,
    Message_MaxLength,
    "+==============================================++======================================================+\n"
  );
  Serial.print(Message);
}

void PrintDetails(me_Wifi::TStation Station, me_Wifi::TChannel Channel)
{
  const uint8_t SecurityName_MaxLength = 40;
  char SecurityName [SecurityName_MaxLength];

  GetSecurityName(
    SecurityName,
    SecurityName_MaxLength,
    Channel.SecurityProtocol
  );

  snprintf(
    Message,
    Message_MaxLength,
    "| %-20s | %s | %s || %4d | %2u | %-40s |\n",
    Station.Name,
    Station.Id,
    Station.IsHidden ? "X" : "-",
    Channel.SignalStrength,
    Channel.BandNumber,
    SecurityName
  );
  Serial.print(Message);
}

bool GetSecurityName(
  char* Message,
  uint16_t Message_MaxLength,
  me_Wifi::TSecurityProtocol SecurityProtocol
)
{
  using namespace me_Wifi;

  switch (SecurityProtocol)
  {
    case TSecurityProtocol::None:
      snprintf(Message, Message_MaxLength, "None");
      break;

    case TSecurityProtocol::Wep:
      snprintf(Message, Message_MaxLength, "WEP");
      break;

    case TSecurityProtocol::Wpa:
      snprintf(Message, Message_MaxLength, "WPA v1");
      break;

    case TSecurityProtocol::Wpa2:
      snprintf(Message, Message_MaxLength, "WPA v2");
      break;

    case TSecurityProtocol::AnyWpa:
      snprintf(Message, Message_MaxLength, "WPA v1 + WPA v2");
      break;

    case TSecurityProtocol::Unknown:
      snprintf(Message, Message_MaxLength, "(we failed to detect security protocol)");
      break;

    default:
      Serial.printf("@ Unknown security protocol value.");
      return false;
  }

  return true;
}

/*
  2023-12-26
  2023-12-28
*/
