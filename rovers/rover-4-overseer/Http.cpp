// Http-server easy-to-use functions

/*
  Status: stable
  Version: 2
  Last mod.: 2023-11-13
*/

#include "Http.h"

ESP8266WebServer HttpServer;

// ---

void NotFoundHandler_Callback();

using namespace Http;

// Start HTTP server with given callback function
void Http::Setup(THandlerFunction RootHandler_Callback)
{
  HttpServer.on("/", RootHandler_Callback);
  HttpServer.onNotFound(NotFoundHandler_Callback);

  HttpServer.begin();
}

// Main loop handler
void Http::HandleEvents()
{
  HttpServer.handleClient();
}

// Send string as plaintext
void Http::SendString(String DataString)
{
  HttpServer.send(
    Response_Ok,
    Content_Plaintext,
    DataString
  );
}

// Get string with client IP address
String Http::GetClientIp()
{
  return HttpServer.client().remoteIP().toString();
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
*/
