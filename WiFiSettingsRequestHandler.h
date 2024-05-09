#include <Arduino.h>
#include <ESP8266WebServer.h>

class WiFiSettingsRequestHandler : public RequestHandler {
  public:
    WiFiSettingsRequestHandler(const char* uri = "update_wifi");
    bool canHandle(HTTPMethod method, const String& uri) override;
    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;

  protected:
    String _uri;  
};