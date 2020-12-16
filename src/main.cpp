#include <Arduino.h>

#include <TFT_eSPI.h>
#define BUTTON1PIN 35
#define BUTTON2PIN 0
#define DEBOUNCE_DELAY 200

void debounce(long * lastDebounce, void (*handler)()) {
  if ((millis() - *lastDebounce) > DEBOUNCE_DELAY) {
    handler();
    *lastDebounce += millis();
  }
}

TFT_eSPI tft = TFT_eSPI();

float t = 0;
float dir = 1;
long lastDebounceButton1 = 0;
long lastDebounceButton2 = 0;

void button1Handler() {
    Serial.println("Button 1 Pressed!");
    dir = 1;
}

void button2Handler() {
    Serial.println("Button 2 Pressed!");
    dir = -1;
}

void IRAM_ATTR toggleButton1() {
  debounce(&lastDebounceButton1, &button1Handler);
}

void IRAM_ATTR toggleButton2() {
  debounce(&lastDebounceButton2, &button2Handler);
}

void setup() {
  Serial.begin(1000000);
  Serial.println("Starting Counter 1 Program");
  tft.begin();
  tft.setRotation(1);
}

void loop() {
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
    
  Serial.println(t);
  t += dir;
}