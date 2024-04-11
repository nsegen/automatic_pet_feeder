#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

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
      "<form action='/save'  method='post'>"
        "<label for='ssid'>SSID:</label><br>"
        "<input type='text' id='ssid' name='ssid' value='John'><br>"
        "<label for='pwd'>Password:</label><br>"
        "<input type='text' id='pwd' name='pwd' value='PWD'><br>"
        "<input type='submit' value='Submit'>"
      "</form>"
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
  Serial.print("Configuring access point...");
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
  webServer.begin();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}
