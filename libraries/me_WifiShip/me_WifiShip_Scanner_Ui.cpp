#include "me_WifiShip_Scanner_Ui.h"

#include <me_Types.h>

#include <cstdio> // snprintf()
#include <HardwareSerial.h> // Serial for debug

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
      "Signal | Name \n"
      "-------+-----------------------------------------------\n"
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
      "-------------------------------------------------------\n"
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
  snprintf(
    Message,
    Message_MaxLength,
    "%6d | %s \n",
    Station.Channel.Strength,
    Station.Name
  );
}
