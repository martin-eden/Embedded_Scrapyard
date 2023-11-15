// Http-server easy-to-use functions

/*
  Status: stable
  Version: 2
  Last mod.: 2023-11-13
*/

#include "Http.h"

ESP8266WebServer HttpServer;

// ---

using namespace Http;

const uint16_t Response_Ok = 200;
const uint16_t Response_NotFound = 404;
const char
  Content_Plaintext[] = "text/plain",
  Content_Html[] = "text/html";


void NotFoundHandler_Callback();

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
