#ifndef FEEDINFOHNDLR_H
#define FEEDINFOHNDLR_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

class FeedInfoRequestHandler : public RequestHandler {
  private:
    bool handle(ESP8266WebServer &server);
  public:
    FeedInfoRequestHandler(const char* uri = "deviceInfo");
    bool canHandle(HTTPMethod method, const String& uri) override;
    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;

  protected:
    String _uri;
};

#endif