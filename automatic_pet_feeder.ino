/*
  Скетч к проекту "Автокормушка 2"
  - Страница проекта (схемы, описания): https://alexgyver.ru/gyverfeed2/
  - Исходники на GitHub: https://github.com/AlexGyver/GyverFeed2/
  Проблемы с загрузкой? Читай гайд для новичков: https://alexgyver.ru/arduino-first/
  AlexGyver, AlexGyver Technologies, 2020
*/

#define FEED_SPEED 3000   // задержка между шагами мотора (мкс)
#define STEPS_FRW 19        // шаги вперёд
#define STEPS_BKW 12        // шаги назад#include <ESP8266WiFi.h>
#define FEED_AMOUNT 1000;
#define DEBUG false
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
const char ap_mode_ssid[] = "automatic_pet_feeder";
const char ap_mode_password[] = "automatic_pet_feeder";
const byte drvPins[] = { D5, D6, D7, D8 };//{ 14, 12, 13, 15 };  // драйвер (фазаА1, фазаА2, фазаВ1, фазаВ2) from D5 to D8
const byte steps[] = {0b1010, 0b0110, 0b0101, 0b1001};
ESP8266WebServer webServer(80);
WiFiManager wifiManager;

class WiFiSettingsRequestHandler : public RequestHandler {
  public:
    WiFiSettingsRequestHandler(const char* uri = "update_wifi") : _uri(uri)
    {
    }
    bool canHandle(HTTPMethod method, const String& uri) override
    { 
      Serial.print("[WiFiSettingsRequestHandler.canHandle] " + uri);
      Serial.print(method);
      Serial.print(": " + uri);

      return method == HTTP_POST && uri == _uri;
    }

    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override 
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

  protected:
    String _uri;  
};

class Feeder {
  private:
    const byte *drvPins;
    const byte *steps;
    short feedSpeed;   // задержка между шагами мотора (мкс)
    short feedAmount;
    short stepsFrw;       // шаги вперёд
    short stepsBkw;
    bool shouldFeed;
    
    // выключаем ток на мотор
    void disableMotor() {
      for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], 0);
    }

    void oneRev() {
      for (int i = 0; i < stepsBkw; i++) runMotor(-1);
      for (int i = 0; i < stepsFrw; i++) runMotor(1);
    }

    void runMotor(short dir) {
      static byte step = 0;
      for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], bitRead(steps[step & 0b11], i));
      delayMicroseconds(feedSpeed);
      step += dir;
    }

  public:
    Feeder(const byte motorPins[4], const byte motorSteps[4]) : drvPins(motorPins), steps(motorSteps) {
      feedSpeed = FEED_SPEED;   // задержка между шагами мотора (мкс)
      feedAmount = FEED_AMOUNT;
      stepsFrw = STEPS_FRW;       // шаги вперёд
      stepsBkw = STEPS_BKW;
      shouldFeed = false;
    }
    
    short getFeedSpeed() { return feedSpeed; }
    void setFeedSpeed(short speed) { feedSpeed = speed; }

    short getFeedAmount() { return feedAmount; }
    void setFeedAmount(short amount) { feedAmount = amount; }
    
    short getStepsFrw() { return stepsFrw; }
    void setStepsFrw(short steps) { stepsFrw = steps; }
    
    short getStepsBkw() { return stepsBkw; }
    void setStepsBkw(short steps) { stepsBkw = steps; }

    void feed() { shouldFeed = true; }
    void handleFeeder() {
      if (shouldFeed) {
        for (int i = 0; i < feedAmount; i++) oneRev();
        disableMotor();
        shouldFeed = false;
      }
    }
};

Feeder feeder(drvPins, steps);

class FeedConfigRequestHandler : public RequestHandler {
  private:
    short feedValueExtract(JsonDocument& doc, const char* prop) {
        short val =  doc[prop].as<short>();
        
        Serial.print("[FeedRequestHandler.feedValueExtract] ");
        Serial.print(prop);
        Serial.print(" is ");
        Serial.println(val);
        
        return val;
    }
  public:
    FeedConfigRequestHandler(const char* uri = "feedConfig") : _uri(uri)
    {
    }
    bool canHandle(HTTPMethod method, const String& uri) override
    { 
      Serial.print("[FeedConfigRequestHandler.canHandle] ");
      Serial.print(method);
      Serial.println(": " + uri);

      return method == HTTP_POST && uri == _uri;
    }

    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override 
    { 
      Serial.println("[FeedConfigRequestHandler.handle] " + requestUri);
      if (!canHandle(requestMethod, requestUri)) {
        return false;
      }
      String jsonString = server.arg("plain");
      JsonDocument doc;

      DeserializationError error = deserializeJson(doc, jsonString);
      if (doc.containsKey("feedSpeed")) {
        feeder.setFeedSpeed(feedValueExtract(doc, "feedSpeed"));
      }
      if (doc.containsKey("feedAmount")) {
        feeder.setFeedAmount(feedValueExtract(doc, "feedAmount"));
      }
      if (doc.containsKey("feedSpeed")) {
        feeder.setStepsFrw(feedValueExtract(doc, "stepsFrw"));
      }
      if (doc.containsKey("feedSpeed")) {
        feeder.setStepsBkw(feedValueExtract(doc, "stepsBkw"));
      }
      return true;
    }

  protected:
    String _uri;  
};

class FeedRequestHandler : public RequestHandler {
  public:
    FeedRequestHandler(const char* uri = "feed") : _uri(uri)
    {
    }
    bool canHandle(HTTPMethod method, const String& uri) override
    { 
      Serial.print("[FeedRequestHandler.canHandle] ");
      Serial.print(method);
      Serial.println(": " + uri);

      return method == HTTP_POST && uri == _uri;
    }

    bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override 
    { 
      Serial.println("[FeedRequestHandler.handle] " + requestUri);
      if (!canHandle(requestMethod, requestUri)) {
        return false;
      }      
      feeder.feed();
      return true;
    }

  protected:
    String _uri;  
};

void setup() {
  Serial.begin(115200);
  delay(1000); 
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    

  #if (DEBUG)
    Serial.print("Debug mode. Reset settings.");
    wifiManager.resetSettings();
  #endif
  wifiManager.setConfigPortalBlocking(false);
  // wifiManager.setConfigPortalTimeout(60);
  wifiManager.setDebugOutput(true);
  wifiManager.setConnectTimeout(60);
  wifiManager.setCaptivePortalEnable(true);
  wifiManager.setWebPortalClientCheck(true);
  wifiManager.setWiFiAutoReconnect(true);
  // wifiManager.setConnectRetries(3);
  if (wifiManager.autoConnect(ap_mode_ssid, ap_mode_password)) {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("[setup] Configure 'update_wifi' URL handler.");
  } else {
      Serial.println("Configportal running");
  }
  webServer.addHandler(new FeedRequestHandler("/feed"));
  webServer.addHandler(new FeedConfigRequestHandler("/feedConfig"));
  Serial.println("[setup] Start web server.");
  webServer.begin();
  Serial.println("[setup] Web server started.");
  for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
}

void loop() {
  wifiManager.process();
  webServer.handleClient();
  feeder.handleFeeder();
}