#include <ArduinoJson.h>
#include "FeedInfoRequestHandler.h"

#define VERSION "0.0.1"
#define MODEL "Automatic Pet Feeder"
#define MANUFACTURER "Segen Nikita"

bool FeedInfoRequestHandler::handleGet(ESP8266WebServer &server) {
  JsonDocument doc;

  doc["model"] = MODEL;
  doc["version"] = VERSION;
  doc["manufacturer"] = MANUFACTURER;

  String result;
  serializeJson(doc, result);
  server.send(200, "application/json", result.c_str());
  return true;
}

FeedInfoRequestHandler::FeedConfigRequestHandler(const char* uri /*= "deviceInfo"*/) : _uri(uri)
{
}

bool FeedInfoRequestHandler::canHandle(HTTPMethod method, const String& uri)
{ 
  Serial.print("[FeedInfoRequestHandler.canHandle] ");
  Serial.print(method);
  Serial.println(": " + uri);

  return uri == _uri;
}

bool FeedInfoRequestHandler::handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) 
{ 
  Serial.println("[FeedInfoRequestHandler.handle] " + requestUri);
  if (!canHandle(requestMethod, requestUri)) {
    return false;
  }
  return handle(server);
}