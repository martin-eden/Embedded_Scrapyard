// WifiShip scanner custom datatypes representation.

/*
  Status: redesigning
  Version: 2
  Last mod.: 2024-02-20
*/

/*
  Design

    RepresentSecurityProtocol
      (Str, StrLen, Protocol_Enum)
*/

#pragma once

#include "Scanner/Interface.h"

namespace me_WifiShip_Scanner_Ui
{
  void RepresentSecurityProtocol(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Scanner::TSecurityProtocol MapSecurityProtocol
  );

  void RepresentScanResult(
    TChar* Message,
    TUint_2 Message_MaxLength,
    me_WifiShip_Scanner::TStation* Stations,
    TUint_1 NumStations
  );
}

/*
  2024-02-07
*/
