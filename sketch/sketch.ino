/*
  Скетч к проекту "Автокормушка 2"
  - Страница проекта (схемы, описания): https://alexgyver.ru/gyverfeed2/
  - Исходники на GitHub: https://github.com/AlexGyver/GyverFeed2/
  Проблемы с загрузкой? Читай гайд для новичков: https://alexgyver.ru/arduino-first/
  AlexGyver, AlexGyver Technologies, 2020
*/

#define DEBUG false
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "Feeder.h"
#include "WiFiSettingsRequestHandler.h"
#include "FeedConfigRequestHandler.h"
#include "FeedRequestHandler.h"

const char ap_mode_ssid[] = "automatic_pet_feeder";
const char ap_mode_password[] = "automatic_pet_feeder";
const byte drvPins[] = { D5, D6, D7, D8 };//{ 14, 12, 13, 15 };  // драйвер (фазаА1, фазаА2, фазаВ1, фазаВ2) from D5 to D8
const byte steps[] = {0b1010, 0b0110, 0b0101, 0b1001};
ESP8266WebServer webServer(80);
WiFiManager wifiManager;
Feeder feeder(drvPins, steps);

struct EEPROMFeederConfig {
  FeederConfig config;
  int hashSum;
};

void printVal(const char* prop, int val) {
  Serial.print(prop);
  Serial.print(" is ");
  Serial.println(val);
}

void printConfig(FeederConfig config) {
  printVal("config.feedSpeed", config.feedSpeed);
  printVal("config.feedAmount", config.feedAmount);
  printVal("config.stepsFrw", config.stepsFrw);
  printVal("config.stepsBkw", config.stepsBkw);
}

int feederConfigHash(FeederConfig config) {
  return config.feedAmount / 4 
       + config.feedSpeed / 4 
       + config.stepsBkw / 4 
       + config.stepsFrw / 4;
}

void readFeederConfigFromEEPROM() {
  EEPROMFeederConfig eepromConfig;
  
  EEPROM.get(0, eepromConfig);
  Serial.print("hashSum from EEPROM: ");
  Serial.println(eepromConfig.hashSum);

  EEPROM.get(0, eepromConfig);
  printConfig(eepromConfig.config);
  int calculatedHashSum = feederConfigHash(eepromConfig.config);

  Serial.print("calculated hashSum: ");
  Serial.println(calculatedHashSum);

  if (eepromConfig.hashSum == calculatedHashSum) {
    Serial.println("Apply stored config from EEPROM.");
    feeder.setConfig(eepromConfig.config);
  } else {
    Serial.println("Feeder config is not stored in EEPROM.");
  }
}

void writeFeederConfigFromEEPROM(FeederConfig config) {
  EEPROMFeederConfig toSave;

  toSave.hashSum = feederConfigHash(config);
  toSave.config = config;

  Serial.print("HashSum for storing in the EEPROM: ");
  Serial.println(toSave.hashSum);

  EEPROM.put(0, toSave);
  EEPROM.commit();
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(sizeof(FeederConfig) + sizeof(int));
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
  readFeederConfigFromEEPROM();
  webServer.addHandler(new FeedRequestHandler(feeder, "/feed"));
  webServer.addHandler(new FeedConfigRequestHandler(feeder, "/feedConfig"));
  webServer.addHandler(new FeedInfoRequestHandler("/deviceInfo"));
  Serial.println("[setup] Start web server.");
  webServer.begin();
  Serial.println("[setup] Web server started.");
  for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
}

void loop() {
  int hashSum1 = feederConfigHash(feeder.getConfig());

  wifiManager.process();
  webServer.handleClient();
  feeder.handleFeeder();
  int hashSum2 = feederConfigHash(feeder.getConfig());

  if (hashSum1 != hashSum2) {
    writeFeederConfigFromEEPROM(feeder.getConfig());
  }
  delay(500);
}