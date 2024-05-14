#ifndef FEEDCONFIGHNDLR_H
#define FEEDCONFIGHNDLR_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "Feeder.h"

class FeedConfigRequestHandler : public RequestHandler {
  private:
    short feedValueExtract(JsonDocument& doc, const char* prop);
    bool handleGet(ESP8266WebServer &server);
    bool handlePost(ESP8266WebServer &server);
  public:
    FeedConfigRequestHandler(Feeder &feeder, const char* uri = "feedConfig");
    bool canHandle(HTTPMethod method, const String& uri) override;
    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;

  protected:
    String _uri;
    Feeder& _feeder;
};

#endif