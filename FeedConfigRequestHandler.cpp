#include <ArduinoJson.h>
#include "FeedConfigRequestHandler.h"

short FeedConfigRequestHandler::feedValueExtract(JsonDocument& doc, const char* prop) {
  short val =  doc[prop].as<short>();
  
  Serial.print("[FeedRequestHandler.feedValueExtract] ");
  Serial.print(prop);
  Serial.print(" is ");
  Serial.println(val);
  
  return val;
}

bool FeedConfigRequestHandler::handleGet(ESP8266WebServer &server) {
  JsonDocument doc;

  doc["feedSpeed"] = _feeder.getFeedSpeed();
  doc["feedAmount"] = _feeder.getFeedAmount();
  doc["stepsFrw"] = _feeder.getStepsFrw();
  doc["stepsBkw"] = _feeder.getStepsBkw();

  String result;
  serializeJson(doc, result);
  server.send(200, "application/json", result.c_str());
  return true;
}

bool FeedConfigRequestHandler::handlePost(ESP8266WebServer &server) {
  String jsonString = server.arg("plain");
  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    server.send(400, "text/plain", error.c_str());

    return false;
  }
  if (doc.containsKey("feedSpeed")) {
    _feeder.setFeedSpeed(feedValueExtract(doc, "feedSpeed"));
  }
  if (doc.containsKey("feedAmount")) {
    _feeder.setFeedAmount(feedValueExtract(doc, "feedAmount"));
  }
  if (doc.containsKey("feedSpeed")) {
    _feeder.setStepsFrw(feedValueExtract(doc, "stepsFrw"));
  }
  if (doc.containsKey("feedSpeed")) {
    _feeder.setStepsBkw(feedValueExtract(doc, "stepsBkw"));
  }
  server.send(200);
  return true;
}

FeedConfigRequestHandler::FeedConfigRequestHandler(Feeder &feeder, const char* uri /*= "feedConfig"*/) : _feeder(feeder), _uri(uri)
{
}

bool FeedConfigRequestHandler::canHandle(HTTPMethod method, const String& uri)
{ 
  Serial.print("[FeedConfigRequestHandler.canHandle] ");
  Serial.print(method);
  Serial.println(": " + uri);

  return (method == HTTP_POST || method == HTTP_GET) && uri == _uri;
}

bool FeedConfigRequestHandler::handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) 
{ 
  Serial.println("[FeedConfigRequestHandler.handle] " + requestUri);
  if (!canHandle(requestMethod, requestUri)) {
    return false;
  }
  if (requestMethod == HTTP_GET) {
    return handleGet(server);
  }
  handlePost(server);
  return true;
}