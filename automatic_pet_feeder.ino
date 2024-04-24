#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
const char ap_mode_ssid[] = "automatic_pet_feeder";
const char ap_mode_password[] = "automatic_pet_feeder";

class WiFiSettingsRequestHandler : public RequestHandler {
  public:
    WiFiSettingsRequestHandler(const char* uri = "update_wifi") : _uri(uri)
    {
    }
    bool canHandle(HTTPMethod method, const String& uri) override
    { 
      Serial.println("[WiFiSettingsRequestHandler.canHandle] " + uri);
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
ESP8266WebServer webServer(80);
// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState = "off";

// Assign output variables to GPIO pins
char output[2] = "5";


String responseHTML = ""
  "<!DOCTYPE html><html lang='en'>"
    "<head>"
      "<meta name='viewport' content='width=device-width' />"
      "<title>CaptivePortal</title>"
    "</head>"
    "<body>"
      "<h1>Enter the WiFi SSID and Password:</h1>"
      "<form name='wifi_info'>"
        "<div><input name='ssid' value='Keenetic-6112' /></div>"
        "<div><input name='password' type='password' value='zLaN9DYZ' /></div>"
        "<div><input type='button' value='update' onclick='update_wifi()' /></div>"
      "</form>"
      "<script>"
        "function update_wifi()"
        "{"
          "let obj = {};"
          "let formData = new FormData(document.forms.wifi_info);"
          "formData.forEach(function(value, key) {"
            "obj[key] = value;"
          "});"
          "let json = JSON.stringify(obj);"          
          "let xhr = new XMLHttpRequest();"

          "xhr.open('POST', '/update_wifi');"
          "xhr.setRequestHeader('Content-type', 'application/json; charset=utf-8');"
          "xhr.send(json);"

          "xhr.onload = () => alert(xhr.response);"
        "}"
      "</script>"
    "</body>"
  "</html>";

void saveWifiInfo() {
  Serial.println("Handle save wifi request.");
  String message = "<h1>\n";
         message += "You are connected\n";
         message += "SSID: ";
         message += webServer.arg("ssid");
         message += "\nPassword: ";
         message += webServer.arg("pwd");
         message += "\n</h1>";
  webServer.send(200, "text/html", message);
}



void setup() {
  Serial.begin(115200);
  delay(100); 
  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);
  wifiManager.autoConnect(ap_mode_ssid, ap_mode_password);
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("[setup] Configure 'not found' URL handler.");
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  Serial.println("[setup] Configure 'save' URL handler.");
  webServer.on("/save", saveWifiInfo);
  Serial.println("[setup] Configure 'update_wifi' URL handler.");
  webServer.addHandler(new WiFiSettingsRequestHandler("/update_wifi"));
  Serial.println("[setup] Start web server.");
  webServer.begin();
  Serial.println("[setup] Web server started.");
  delay(500);
}

void loop() {
  webServer.handleClient();
  delay(500);
}
