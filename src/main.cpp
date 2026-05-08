#include <Arduino.h>
#include "bluetooth.h"
#include "buzzer.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>

// OLED screen setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Soil moisture sensor analog pin
// On the Maker Board, this corresponds to 32 / A4.
const int MOISTURE_SENSOR_PIN = 32;

// NeoPixel ring setup
const int NEOPIXEL_PIN = 25;   // Maker Board NeoPixel connector
const int NUM_PIXELS = 16;     // Change if your ring has a different number of LEDs
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Calibration values
int dryValue = 2348;
int wetValue = 1200;

// Timing
// Sensor will be read every 2 seconds
const unsigned long READ_INTERVAL = 2000;
unsigned long lastReadTime = 0;

bool redAlertActive = false;
unsigned long lastRedAlertTime = 0;
const unsigned long RED_ALERT_INTERVAL = 30UL * 60UL * 1000UL;

// Function declarations
int moistureToPercent(int rawValue);
int readMoisturePercent();
void showMoistureStatus(int moisturePercent);
void setRingColor(uint8_t red, uint8_t green, uint8_t blue);
void updateScreen(int moisturePercent, String line1, String line2);

void setup() {
  Serial.begin(115200);

  // Start buzzer
  setupBuzzer();

  // Start NeoPixel ring
  pixels.begin();
  pixels.clear();
  pixels.show();

  // Start OLED screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Screen not found!");
    while (true);
  }

  // Start Bluetooth
  beginBluetooth();

  // Welcome message
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("Smart");
  display.setCursor(10, 35);
  display.println("Plant :)");
  display.display();

  delay(2000);

  Serial.println("Smart Plant Monitor started.");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastReadTime >= READ_INTERVAL) {
    lastReadTime = currentTime;

    int moisturePercent = readMoisturePercent();

    Serial.print("Moisture percentage: ");
    Serial.print(moisturePercent);
    Serial.println("%");

    showMoistureStatus(moisturePercent);

    // Faith's Bluetooth helper sends connection message and timed updates
    sendStatusUpdate(moisturePercent, currentTime);
  }
}

int readMoisturePercent() {
  long total = 0;
  const int samples = 10;

  for (int i = 0; i < samples; i++) {
    total += analogRead(MOISTURE_SENSOR_PIN);
    delay(10);
  }

  int rawValue = total / samples;

  Serial.print("Raw moisture value: ");
  Serial.println(rawValue);

  return moistureToPercent(rawValue);
}

int moistureToPercent(int rawValue) {
  int moisturePercent = map(rawValue, dryValue, wetValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  return moisturePercent;
}

void showMoistureStatus(int moisturePercent) {
  int moistureBand;

  if (moisturePercent <= 25) {
    moistureBand = 0; // RED - critically dry

    setRingColor(255, 0, 0);
    Serial.println("Status: RED - Plant needs water urgently.");

    unsigned long currentTime = millis();
    if (!redAlertActive) {
      sendRedZoneAlert();
      redAlertActive = true;
      lastRedAlertTime = currentTime;
    } else if (currentTime - lastRedAlertTime >= RED_ALERT_INTERVAL) {
      sendRedZoneAlert();
      lastRedAlertTime = currentTime;
    }

    updateScreen(moisturePercent, "Thirsty :(", "Water me now!");
  } 
  else if (moisturePercent <= 50) {
    moistureBand = 1; // YELLOW - getting dry

    setRingColor(255, 180, 0);
    Serial.println("Status: YELLOW - Plant needs water soon.");

    updateScreen(moisturePercent, "Getting dry", "Water me soon!");
  } 
  else if (moisturePercent <= 80) {
    moistureBand = 2; // GREEN - healthy

    setRingColor(0, 255, 0);
    Serial.println("Status: GREEN - Plant has enough water.");

    updateScreen(moisturePercent, "I feel", "refreshed");
  } 
  else {
    moistureBand = 3; // BLUE - overwatered

    setRingColor(0, 0, 255);
    Serial.println("Status: BLUE - Plant has too much water.");

    updateScreen(moisturePercent, "Too wet!", "Let me dry out");
  }

  if (moisturePercent > 25) {
    redAlertActive = false;
  }
  
  // Buzzer beeps only when moistureBand is 0 / red zone
  updateBuzzer(moistureBand);
}

void setRingColor(uint8_t red, uint8_t green, uint8_t blue) {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }

  pixels.show();
}

void updateScreen(int moisturePercent, String line1, String line2) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(line1);

  display.setTextSize(1);
  display.setCursor(0, 24);
  display.println(line2);

  display.drawLine(0, 36, 128, 36, WHITE);

  display.setTextSize(2);
  display.setCursor(0, 42);
  display.print(moisturePercent);
  display.println("%");

  display.display();
}





