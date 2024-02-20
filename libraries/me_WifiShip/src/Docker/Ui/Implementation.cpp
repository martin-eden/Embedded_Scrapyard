// Last mod.: 2024-02-20

#include "Docker/Ui/Interface.h"

#include <cstdio> // snprintf()

void me_WifiShip_Docker_Ui::RepresentStatus(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Docker::TStatus Status
)
{
  using namespace me_WifiShip_Docker;
  switch (Status)
  {
    case TStatus::Undocked:
      snprintf(Message, Message_MaxLength, "Undocked");
      break;

    case TStatus::Docked:
      snprintf(Message, Message_MaxLength, "Docked");
      break;

    case TStatus::Nah_StationNotFound:
      snprintf(Message, Message_MaxLength, "Station not found");
      break;

    case TStatus::Nah_WrongPassword:
      snprintf(Message, Message_MaxLength, "Wrong password");
      break;

    case TStatus::Nah_TimeoutReached:
      snprintf(Message, Message_MaxLength, "Timeout reached");
      break;

    case TStatus::Nah_Other:
      snprintf(Message, Message_MaxLength, "Other");
      break;

    default:
      snprintf(Message, Message_MaxLength, "@(Not covered)");
  }
}

// Represent IP address
void me_WifiShip_Docker_Ui::RepresentAddress(
  TChar* Message,
  TUint_2 Message_MaxLength,
  me_WifiShip_Docker::TAddress Address
)
{
  snprintf(
    Message,
    Message_MaxLength,
    "%u.%u.%u.%u",
    Address[0],
    Address[1],
    Address[2],
    Address[3]
  );
}
