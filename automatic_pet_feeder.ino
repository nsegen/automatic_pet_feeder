#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

struct WiFiSettings
{
  String ssid;
  String password;
};

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
      Serial.println("[WiFiSettingsRequestHandler.handle] " + jsonString);
      server.send(200, "application/json", jsonString);
      
      return true;
    }

  protected:
    String _uri;  
};

/* Set these to your desired credentials. */
const char *ssid = "pet_feeder";
const char *password = "pet_feeder";
const byte DNS_PORT = 53;

IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String responseHTML = ""
  "<!DOCTYPE html><html lang='en'>"
    "<head>"
      "<meta name='viewport' content='width=device-width' />"
      "<title>CaptivePortal</title>"
    "</head>"
    "<body>"
      "<h1>Enter the WiFi SSID and Password:</h1>"
      "<form name='wifi_info'>"
        "<input name='ssid' value='John'>"
        "<input name='password' value='Smith'>"
        "<input type='button' value='update' onclick='update_wifi()'>"
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
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("WiFiSettings struct size is ");
  Serial.println(sizeof(WiFiSettings));
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // replay to all requests with same HTML
  webServer.onNotFound([]() {
    webServer.send(200, "text/html", responseHTML);
  });
  webServer.on("/save", saveWifiInfo);
  webServer.addHandler(new WiFiSettingsRequestHandler("/update_wifi"));
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
