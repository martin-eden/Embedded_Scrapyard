// WifiShip assembly

#include "me_WifiShip.h"

#include <ESP8266WiFi.h> // ESP8266 official SDK

using namespace me_WifiShip;

// --( Init )--
TBool TWifiShip::Init()
{
  {
    bool Inner_Enable = true;
    WiFi.enableSTA(Inner_Enable);
  }

  return
    Core.Init() &&
    Scanner.Init() &&
    Docker.Init();
}

// --

/*
  2024-01-01
  2024-01-03
*/
