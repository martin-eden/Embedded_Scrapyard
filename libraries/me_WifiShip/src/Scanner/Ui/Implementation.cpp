// WifiShip scanner custom datatypes representation. Implementation.

/*
  Status: redesign
  Version: 2
  Last mod.: 2024-02-20
*/

#include "Scanner/Ui/Interface.h"

#include <cstdio> // snprintf()
#include <HardwareSerial.h> // Serial for debug

#include "Common/CraftIdentity/Ui/Interface.h" // Represent CraftId

void RepresentSecurityProtocol(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Scanner::TSecurityProtocol SecurityProtocol
)
{
  using namespace me_WifiShip_Scanner;

  switch (SecurityProtocol)
  {
    case TSecurityProtocol::None:
      snprintf(Message, Message_MaxLength, "None");
      break;

    case TSecurityProtocol::Wep:
      snprintf(Message, Message_MaxLength, "WEP");
      break;

    case TSecurityProtocol::Wpa:
      snprintf(Message, Message_MaxLength, "WPA");
      break;

    case TSecurityProtocol::Wpa2:
      snprintf(Message, Message_MaxLength, "WPA2");
      break;

    case TSecurityProtocol::AnyWpa:
      snprintf(Message, Message_MaxLength, "WPA1/2");
      break;

    case TSecurityProtocol::Unknown:
      snprintf(Message, Message_MaxLength, "Unknown");
      break;

    default:
      snprintf(Message, Message_MaxLength, "@(Not covered)");
  }

}

// Helper function to represent one station found by scan
void RepesentStation(TChar* Message, TUint_2 Message_MaxLength, me_WifiShip_Scanner::TStation Station);

void me_WifiShip_Scanner_Ui::RepresentScanResult(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Scanner::TStation* Stations,
  TUint_1 NumStations
)
{
  Message[0] = '\0';

  TUint_1 RowStr_Size = 200;
  TChar RowStr[RowStr_Size];

  if (NumStations == 0)
    return;

  snprintf(
    RowStr,
    RowStr_Size,
    PSTR(
      "Signal | Protocol |        Id         |           Name\n"
      "-------+----------+-------------------+---------------------------------\n"
    )
  );
  strlcat(Message, RowStr, Message_MaxLength);

  for (TUint_1 StationIndex = 0; StationIndex < NumStations; ++StationIndex)
  {
    RepesentStation(RowStr, RowStr_Size, Stations[StationIndex]);
    strlcat(Message, RowStr, Message_MaxLength);
  }

  snprintf(
    RowStr,
    RowStr_Size,
    PSTR(
      "------------------------------------------------------------------------\n"
      "%u stations\n"
    ),
    NumStations
  );
  strlcat(Message, RowStr, Message_MaxLength);
}

// Helper function to represent one station found by scan
void RepesentStation(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Scanner::TStation Station
)
{
  const TUint_1 ProtocolName_Size = 20;
  TChar ProtocolName[ProtocolName_Size];

  RepresentSecurityProtocol(ProtocolName, ProtocolName_Size, Station.Channel.Security);

  const TUint_1 IdStr_Size = 20;
  TChar IdStr[IdStr_Size];

  me_WifiShip_Common_CraftIdentity_Ui::RepresentCratfId(IdStr, IdStr_Size, Station.Id);

  snprintf(
    Message,
    Message_MaxLength,
    "%6d | %-8s | %17s | %s \n",
    Station.Channel.Strength,
    ProtocolName,
    IdStr,
    Station.Name
  );
}

/*
  2024-02-07
*/
