#include <Arduino.h>
#include "bluetooth.h"

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

// Red alert timing
bool inRedZone = false;
unsigned long lastRedAlertTime = 0;
const unsigned long RED_ALERT_INTERVAL = 30UL * 60UL * 1000UL;

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
    
    unsigned long currentTime = millis();

    if (!inRedZone) {
      sendRedZoneAlert();
      inRedZone = true;
      lastRedAlertTime = currentTime;
    } else if (currentTime - lastRedAlertTime >= RED_ALERT_INTERVAL) {
      sendRedZoneAlert();
      lastRedAlertTime = currentTime;
    }
  } 

  else if (moisturePercent <= 50) {
    digitalWrite(YELLOW_LED_PIN, HIGH);
    Serial.println("Status: YELLOW - Plant needs water soon.");
  } 
  else if (moisturePercent <= 80) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    Serial.println("Status: GREEN - Plant has enough water.");
  } 
  else {
    digitalWrite(BLUE_LED_PIN, HIGH);
    Serial.println("Status: BLUE - Plant has too much water.");
  }
}

//main setup
void setup() {
  Serial.begin(115200);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  turnOffAllLEDs();

  beginBluetooth();

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





