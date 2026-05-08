#include <Arduino.h>
#include "bluetooth.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
//Set-up for the maker board
// Soil moisture sensor analog pin.
// On the Maker Board, this corresponds to 36 / A18.
const int MOISTURE_SENSOR_PIN = 36;

// LED pins connected through resistors.
// These pins are on the Maker Board side header.
const int RED_LED_PIN = 13;      // A14
const int YELLOW_LED_PIN = 14;   // A16
const int GREEN_LED_PIN = 27;    // A17
const int BLUE_LED_PIN = 32;     // A4

//calibration values
int dryValue = 3500;
int wetValue = 1200;

//timing
//sensor will be read every 2 seconds
const unsigned long READ_INTERVAL = 2000;
unsigned long lastReadTime = 0;

//helper functions
void turnOffAllLEDs() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
}

int moistureToPercent(int rawValue) {
  // Convert the raw sensor value into a percentage. dryValue maps to 0%. wetValue maps to 100%.
  int percent = map(rawValue, dryValue, wetValue, 0, 100);
  // result kept between 0 and 100.
  percent = constrain(percent, 0, 100);
  return percent;
}

void showMoistureStatus(int moisturePercent) {
  turnOffAllLEDs();
  if (moisturePercent <= 25) {
    digitalWrite(RED_LED_PIN, HIGH);
    Serial.println("Status: RED - Plant needs water urgently.");

    sendRedZoneAlert();
    updateScreen(moisturePercent, "Thirsty :(", "Water me now!"); // ADD THIS

  } 
  else if (moisturePercent <= 50) {
    digitalWrite(YELLOW_LED_PIN, HIGH);
    Serial.println("Status: YELLOW - Plant needs water soon.");
    updateScreen(moisturePercent, "Getting dry", "Water me soon!"); // ADD THIS

  } 
  else if (moisturePercent <= 80) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("Status: GREEN - Plant has enough water.");
    updateScreen(moisturePercent, "Healthy! :)", "All good!"); // ADD THIS

  } 
  else {
    digitalWrite(BLUE_LED_PIN, HIGH);
    Serial.println("Status: BLUE - Plant has too much water.");
     updateScreen(moisturePercent, "Too wet!", "Let me dry out"); // ADD THIS

  }
}
// function for the screen's setup

void updateScreen(int moisturePercent, String line1, String line2) {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(line1);

  display.setTextSize(1);
  display.setCursor(0, 22);
  display.println(line2);

  display.drawLine(0, 33, 128, 33, WHITE);

  display.setTextSize(2);
  display.setCursor(0, 38);
  display.print(moisturePercent);
  display.println("% water");

  display.display();
}
//main setup
void setup() {
  Serial.begin(115200);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  turnOffAllLEDs();
// Start screen
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Screen not found!");
    while (true);
  }

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

    int rawMoisture = analogRead(MOISTURE_SENSOR_PIN);
    int moisturePercent = moistureToPercent(rawMoisture);

    sendStatusUpdate(moisturePercent, currentTime);

    Serial.print("Raw moisture value: ");
    Serial.println(rawMoisture);

    Serial.print("Moisture percentage: ");
    Serial.print(moisturePercent);
    Serial.println("%");

    showMoistureStatus(moisturePercent);

    Serial.println("----------------------");
  }
}





