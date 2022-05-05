#include <DNSServer.h>
#include <ESPUI.h>
#include <WiFiAP.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

//Para modo station
const char* ssid = "ESPUI";
const char* password = "espui";

//Para modo host
const char* hostname = "Barrera";
bool state_barrera = false;
const char* hostpassword = "Reset";

const char *ssid = "Barrera";
const char *password = "reset";



//Defs varios
const uint8_t PIN_BARRERA = 2;

void switchCallback(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        state_barrera = true;
        break;

    case S_INACTIVE:
        state_barrera = false;
        break;
    }
}

void setup(void)
{
    
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

    // You can remove the password parameter if you want the AP to be open.
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    server.begin();

    Serial.println("Server started");
    ESPUI.switcher("Barrera", &switchCallback, ControlColor::None, true);
    ESPUI.begin("Control de la barrera");
}

void loop(void)
{
    Serial.println(state_barrera);
    digitalWrite(PIN_BARRERA, state_barrera == true? HIGH:LOW);
    dnsServer.processNextRequest();
}