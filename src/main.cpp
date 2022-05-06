#include <DNSServer.h>
#include <ESPUI.h>

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
const char* hostpassword = "Reset";

//Defs varios
const uint8_t PIN_BARRERA1 = 2;
const uint8_t PIN_BARRERA2 = 15;

bool state1 = false;
bool state2 = false;

void switchCallback(Control* sender, int value)
{
    switch (value)
    {
    case S_ACTIVE:
        state1 = true;
        state2 = false;
        break;

    case S_INACTIVE:
        state1 = false;
        state2 = true;
        break;
    }
}

void setup(void)
{
    ESPUI.setVerbosity(Verbosity::VerboseJSON);
    Serial.begin(115200);

    pinMode(PIN_BARRERA1, OUTPUT);
    pinMode(PIN_BARRERA2, OUTPUT);

    

#if defined(ESP32)
    WiFi.setHostname(hostname);
#else
    WiFi.hostname(hostname);
#endif

    //Intentamos modo station

    WiFi.begin(ssid, password);
    Serial.print("\n\nTry to connect to existing network");

    {
        uint8_t timeout = 10;

        // Wait for connection, 5s timeout
        do
        {
            delay(500);
            Serial.print(".");
            timeout--;
        } while (timeout && WiFi.status() != WL_CONNECTED);

        // Si no conseguimos conectarnos, activamos modo host
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.print("\n\nCreating hotspot");

            WiFi.mode(WIFI_AP);
            delay(100);
            WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
            WiFi.softAP(hostname, hostpassword);

            timeout = 5;

            do
            {
                delay(500);
                Serial.print(".");
                timeout--;
            } while (timeout);
        }
    }

    dnsServer.start(DNS_PORT, "*", apIP);

    Serial.println("\n\nWiFi parameters:");
    Serial.print("Mode: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
    Serial.print("IP address: ");
    Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());

   
    ESPUI.switcher("Barrera", &switchCallback, ControlColor::None, true);
    ESPUI.begin("Control de la barrera");
}

void loop(void)
{
    digitalWrite(PIN_BARRERA1, state1 == true ? HIGH : LOW);
    digitalWrite(PIN_BARRERA2, state2 == true ? HIGH : LOW);
    dnsServer.processNextRequest();
}