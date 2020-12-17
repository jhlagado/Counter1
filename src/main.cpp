#include <Arduino.h>

const char MAIN_wepage[] PROGMEM = R"=====(
  <html>
    <head>
      <title>Hello world</title>
    </head>
    <body>
      Hello World!
    </body>
  </html>
)=====";

#include <ESPmDNS.h>
#include <TFT_eSPI.h>
#include <WebServer.h>
#include <WiFi.h>

/*
 TODO:
 Create your own file called credentials.h in this folder
 containing your own network's WiFi SSID and password
 For security reasons, do not commit this file to your own repository

 Here is an example credentials.h file:

    const char* SSID = "MyNetwork";
    const char* PASSWORD = "MyPassword";

*/

#include "credentials.h"

#define BUTTON1PIN 35
#define BUTTON2PIN 0
#define DEBOUNCE_DELAY 200

TFT_eSPI tft = TFT_eSPI();
WiFiClient espClient;
WebServer server(80);

float t = 0;
float dir = 1;
long lastDebounceButton1 = 0;
long lastDebounceButton2 = 0;
const char* deviceName = "counter1";

void showNetworkInfo()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setCursor(0, 30);
    tft.setFreeFont(&Orbitron_Light_24);
    tft.println("IP");
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println(WiFi.localIP());
    tft.setCursor(0, 90);
    tft.setFreeFont(&FreeSerif12pt7b);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.print("http://");
    tft.print(deviceName);
    tft.println(".local");
    delay(5000);
}

void printNetworkInfo()
{
    Serial.println();
    Serial.println();
    Serial.print("Connected to: ");
    Serial.println(SSID);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Navigate to http://");
    Serial.print(deviceName);
    Serial.println(".local");
}

void handleRoot()
{
    server.send(200, "text/html", MAIN_wepage);
}

void debounce(long* lastDebounce, void (*handler)())
{
    if ((millis() - *lastDebounce) > DEBOUNCE_DELAY) {
        handler();
        *lastDebounce += millis();
    }
}

void button1Handler()
{
    Serial.println("Count up");
    dir = 1;
}

void button2Handler()
{
    Serial.println("Count down");
    dir = -1;
}

void IRAM_ATTR toggleButton1()
{
    debounce(&lastDebounceButton1, &button1Handler);
}

void IRAM_ATTR toggleButton2()
{
    debounce(&lastDebounceButton2, &button2Handler);
}

void setup()
{
    Serial.begin(1000000);
    Serial.println("Starting Counter1");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.println("Scanning networks");
    int n = WiFi.scanNetworks();
    Serial.println("Scan complete");
    if (n == 0) {
        Serial.println("No networks found");
    } else {
        for (int i = 0; i < n; ++i) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.print(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? " " : "*");
            Serial.println();
            delay(10);
        }
    }

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting WiFi to '");
    Serial.print(SSID);
    Serial.print("'");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print('.');
    }
    if (!MDNS.begin(deviceName)) {
        Serial.print("Error starting MDNS");
        return;
    }
    server.on("/", handleRoot);
    server.begin();

    printNetworkInfo();

    tft.begin();
    tft.setRotation(1);
    showNetworkInfo();
}

void loop()
{
    server.handleClient();
    delay(100);
    pinMode(BUTTON1PIN, INPUT);
    pinMode(BUTTON2PIN, INPUT);
    attachInterrupt(BUTTON1PIN, toggleButton1, FALLING);
    attachInterrupt(BUTTON2PIN, toggleButton2, FALLING);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setFreeFont(&Orbitron_Light_24);
    tft.setCursor(50, 30);
    tft.println("Counter");
    tft.drawLine(0, 40, 250, 40, TFT_BLUE);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setFreeFont(&Orbitron_Light_32);
    tft.setCursor(60, 100);
    tft.println(t);

    t += dir;
}