#include "FeedRequestHandler.h"

FeedRequestHandler::FeedRequestHandler(Feeder &feeder, const char* uri /*= "feed"*/) : _feeder(feeder), _uri(uri)
{
}
bool FeedRequestHandler::canHandle(HTTPMethod method, const String& uri)
{ 
  Serial.print("[FeedRequestHandler.canHandle] ");
  Serial.print(method);
  Serial.println(": " + uri);

  return method == HTTP_POST && uri == _uri;
}

bool FeedRequestHandler::handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) 
{ 
  Serial.println("[FeedRequestHandler.handle] " + requestUri);
  if (!canHandle(requestMethod, requestUri)) {
    return false;
  }      
  _feeder.feed();
  return true;
}