// Http-server easy-to-use functions

/*
  Status: stable
  Version: 3
  Last mod.: 2023-11-15
*/

#include "Http.h"

// ---

ESP8266WebServer HttpServer;

const uint16_t Response_Ok = 200;
const uint16_t Response_NotFound = 404;
const char
  Content_Plaintext[] = "text/plain",
  Content_Html[] = "text/html";

void NotFoundHandler_Callback();

// ---

using namespace Http;

// Start HTTP server
void Http::Setup()
{
  HttpServer.onNotFound(NotFoundHandler_Callback);

  HttpServer.begin();
}

// Add endpoint handler
void Http::AddEndpoint(const char * EndpointPath, THandlerFunction CallbackFunc)
{
  HttpServer.on(EndpointPath, CallbackFunc);
}

// Main loop handler
void Http::HandleEvents()
{
  HttpServer.handleClient();
}

// Send string as plaintext
void Http::SendPlaintext(String DataString)
{
  HttpServer.send(
    Response_Ok,
    Content_Plaintext,
    DataString
  );
}

// Send string as HTML
void Http::SendHtml(String DataString)
{
  HttpServer.send(
    Response_Ok,
    Content_Html,
    DataString
  );
}

// Get string with client IP address
String Http::GetClientIp()
{
  return HttpServer.client().remoteIP().toString();
}


uint16_t Http::Request_GetNumEntities()
{
  return HttpServer.args();
}

String Http::Request_GetEntityName(uint16_t EntityIndex)
{
  return HttpServer.argName(EntityIndex);
}

String Http::Request_GetEntityValueByName(String EntityName)
{
  return HttpServer.arg(EntityName);
}

String Http::Request_GetEntityValueByIndex(uint16_t EntityIndex)
{
  return HttpServer.arg(EntityIndex);
}

// ---

// "Not found" handler
void NotFoundHandler_Callback()
{
  HttpServer.send(
    Response_NotFound,
    Content_Plaintext,
    "No such endpoint.\n\n"
  );
}

// ---

/*
  2023-11-07
  2023-11-09
  2023-11-13
  2023-11-15
*/
