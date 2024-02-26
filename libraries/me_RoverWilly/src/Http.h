// Http-end functions

/*
  Status: stable
  Version: 3
  Last mod.: 2023-11-15
*/

#pragma once

#include <Arduino.h>

#include <ESP8266WebServer.h>

namespace Http
{
  typedef std::function<void (void)> THandlerFunction;

  void Setup();
  void AddEndpoint(const char * EndpointPath, THandlerFunction CallbackFunc);
  void HandleEvents();

  void SendPlaintext(String DataString);
  void SendHtml(String DataString);

  String GetClientIp();

  uint16_t Request_GetNumEntities();
  String Request_GetEntityName(uint16_t EntityIndex);
  String Request_GetEntityValueByName(String EntityName);
  String Request_GetEntityValueByIndex(uint16_t EntityIndex);
}

/*
  2023-11-07
  2023-11-13
  2023-11-15
*/
