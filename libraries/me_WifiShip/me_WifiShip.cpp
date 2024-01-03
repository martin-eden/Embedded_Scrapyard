#include "me_WifiShip.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK
// #include <Arduino.h> // Serial for debug

/*
  -- gift to scannner
  Station(extends Craft) - router that we can try to connect

    IsHidden - has no name, usually not something that welcomes connects

    Channel - offered frequency band. See Channel.

  Channel - radio band channel that is used for data transmission

    Band - number of channel. Physically it describes range of radio
      frequencies (band).

    Strength - received signal strength indicator (RSSI).

      Weird negative integer number that represents logarithm
      of signal-to-noise-ratio.

      Maybe I'll to convert internal representation to something
      more useful.

    SecurityProtocol - WEP, WPA 1, WPA 2 or (WPA 1 + WPA 2).

        WEP - cracked
        WPA 1 - deprecated
        WPA 1 + WPA 2 - deprecated support
        WPA 2 - normal router should use this

    # Actually we can retrieve more channel details but I don't
    # need them for my projects.

  -- gift to docker
  Connection(extends Channel) - we are connected and got IP

    StationAddress - DNS IP. Something like 192.168.0.1.

    ShipAddress - IP. Our IP in local network. Something like
      192.168.0.208.
*/

using namespace me_WifiShip;

TBool TWifiShip::Init()
{
  WiFi.enableSTA(true);

  return
    Core.Init(); // &&
    // Scanner.Init() &&
    // Docker.Init();
}

// --

/*
  2024-01-01
  2024-01-03
*/
