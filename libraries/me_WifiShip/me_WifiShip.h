// Connect to WiFi router

/*
  Status: redesigning
  Version: 5
  Last mod.: 2023-12-31
*/

/*
  System library interface is usually ugly.

  System library needs to support backward compatibility and
  provide access to new features. So it's names and structures
  can remain at 1980 level or they can represent a weird mixture
  of design solutions.

  I do not want that ugliness in my code.

  Ugliness stops here. It's a wrapper over <ESP8266WiFi.h>.
  It does what I need and nothing else. Structures and classes
  named as I wanted. Connection modes that I don't need are
  not supported.

  What is supported?

  Network scan to get list of nearby routers. Connection to router
  by MAC. Retrieving IP address. Disconnection from router.

  Cheers, Ivan Grokhotkov!
*/

#pragma once

#include <me_Types.h>

/*
  Interface terminology

    I just imagining docking process in Elite: Odyssey where we are Ship
    docking to another structure (Station). So for memorable interface
    I'm using these terms:

    | Local   | Common
    +-----------------------------
    | Ship    | Access Point
    | Station | Router
    | Id      | MAC
    | Name    | Name
    | Address | IP

  Design

    This ship has three parts:

      * Core

        Ship core is it's name and id. You can change them.

      * Scanner

        Scanner scans for nearby stations. You need to call it's
        Scan() to retrieve list of stations.

      * Docker

        Docker connects/disconnects to station. You need to know
        station name and password. You can get names from scanner.

  Classes

    WifiShip: TWifiShip
      Core: TCore
      Scanner: TScanner
      Docker: TDocker

      Init(): bool

  Source files

    Parts

      WifiShip - <me_WifiShip.h>
      Core - <me_WifiShip_Core.h>
      Scanner - <me_WifiShip_Scanner.h>
      Docker - <me_WifiShip_Docker.h>

    All four classes have personal companion module:

      * Serial text interface for demo/test.

        Filename suffix: "_Ui"
        E.g.: <me_WifiShip_Core_Ui.h>

        It prints class states nicely. And gets user input when needed
        (ask name/password to connect).

        Not really needed in production code but very welcome in development.

    Demo/test

      <me_WifiShip.ino>
*/

#include <me_WifiShip_Core.h>
// #include <me_WifiShip_Scanner.h>
// #include <me_WifiShip_Docker.h>

namespace me_WifiShip
{
  class TWifiShip
  {
    public:
      me_WifiShip_Core::TWifiShip_Core Core;
      // me_WifiShip::TWifiShip_Scanner Scanner;
      // me_WifiShip::TWifiShip_Docker Docker;

      TBool Init();
  };
}

/*
  2023-11-07
  2023-11-13
  2023-11-14
  2023-12-28 -- code structural design, network scan
  2023-12-31 -- using <me_Types.h>
  2024-01-01 -- splat to three modules: ship = (frame, scanner, docker)
*/
