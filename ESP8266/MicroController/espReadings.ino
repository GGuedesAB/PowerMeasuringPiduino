/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.
   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.
 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

String new_ssid;
String new_password;

ESP8266WebServer server(80);

const int led = 13;

String prepareConnectionPage()
{
  String htmlPage =
     String("") +
            "<html>"+
  "<head>" +
    "<title>Connect to Host</title>" +
    "<style>" +
      "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }" +
    "</style>" +
  "</head>" +
  "<body>" +
    "<h1>Connect to your host AP!</h1>" +
    "<form action='/connection' method='post'>" +
    "<p>Host SSID:</p>" +
    "<input type='text' name='hostSSID'><br>" +
    "<p>Host Password:</p>" +
    "<input type='password' name='hostPSW'><br><br>" +
    "<input type=\"submit\" name=\"submit\" value=\"ENTER\"/>" +
    "</form>" +
  "</body>" +
"</html>";
  return htmlPage;
}

String prepareConfirmationPage(String connected_ssid)
{
  
  String htmlPage =
     String("") +
            "<html>" +
  "<head>" +
    "<title>ESP Connected</title>" +
    "<style>" +
      "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }" +
    "</style>" +
  "</head>" +
  "<body>" +
    "<h1>You're being connected to: " +
    connected_ssid +
    "</h1>" +
  "</body>" +
"</html>";
  return htmlPage;
}

void handleRoot() {
  digitalWrite(led, 1);
  //char temp[600];
  char htmlChar [600];
  String htmlToSend = prepareConnectionPage();
  htmlToSend.toCharArray(htmlChar, 600);
  //snprintf(temp, 600, htmlChar);
  server.send(200, "text/html", htmlChar);
  digitalWrite(led, 0);
}

void handlePost() {
  char temp[600];
  char htmlChar [600];
  char temp_ssid [25];
  char temp_password [100];
  
  new_ssid = String (server.arg(0));
  new_password = String (server.arg(1));
  
  String htmlToSend = prepareConfirmationPage(new_ssid);
  htmlToSend.toCharArray(htmlChar, 600);
  
  server.send(200, "text/html", htmlChar);

  new_ssid.toCharArray(temp_ssid, 25);
  new_password.toCharArray(temp_password, 100);
  //Show connection attempt
  delay(1000);
  
  createNewConnection (temp_ssid, temp_password);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void createNewConnection (char* ssid, char* password) {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void setup(void) {
  //pinMode(led, OUTPUT);
  //digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("");

  boolean result = WiFi.softAP("ESPsoft", "maniot@winet");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.on("/", handleRoot);
  server.on("/connection", handlePost);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
