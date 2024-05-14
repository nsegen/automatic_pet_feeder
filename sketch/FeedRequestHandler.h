#ifndef FEEDHNDLR_H
#define FEEDHNDLR_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "Feeder.h"

class FeedRequestHandler : public RequestHandler {
  public:
    FeedRequestHandler(Feeder &feeder, const char* uri = "feed");
    bool canHandle(HTTPMethod method, const String& uri) override;
    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override ;

  protected:
    String _uri;  
    Feeder& _feeder;
};

#endif