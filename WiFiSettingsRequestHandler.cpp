#ifndef WIFISETTINGSHNDLR_H
#define WIFISETTINGSHNDLR_H

#include <ArduinoJson.h>
#include "WiFiSettingsRequestHandler.h"

WiFiSettingsRequestHandler::WiFiSettingsRequestHandler(const char* uri /* = "update_wifi"*/) : _uri(uri)
{
}

bool WiFiSettingsRequestHandler::canHandle(HTTPMethod method, const String& uri)
{ 
  Serial.print("[WiFiSettingsRequestHandler.canHandle] " + uri);
  Serial.print(method);
  Serial.print(": " + uri);

  return method == HTTP_POST && uri == _uri;
}

bool WiFiSettingsRequestHandler::handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) 
{ 
  Serial.println("[WiFiSettingsRequestHandler.handle] " + requestUri);
  if (!canHandle(requestMethod, requestUri)) {
    return false;
  }
  String jsonString = server.arg("plain");
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, jsonString);
  if (error || !doc.containsKey("ssid") || !doc.containsKey("password"))
  {
    Serial.println(F("Failed to parse a request into wifi configuration."));
    return false;
  }
  char ssid[128];
  char password[128];
  
  strlcpy(ssid, doc["ssid"], sizeof(ssid));    
  strlcpy(password, doc["password"], sizeof(password));
  Serial.print("[WiFiSettingsRequestHandler.handle] SSID: ");
  Serial.print(ssid);
  Serial.print(", PWD: ");
  Serial.println(password);
  return true;
}

#endif