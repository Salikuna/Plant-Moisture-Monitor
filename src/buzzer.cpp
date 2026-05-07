#include <Arduino.h>
#include "buzzer.h"

//Pin Definition
const int BUZZER_PIN = 26;  // GPIO pin that buzzer is wired to 

//Timing
const unsigned long BEEP_INTERVAL = 3600000UL;  
const int BEEP_DURATION_MS = 500;               
const int NUM_BEEPS = 3;                        
const int BEEP_GAP_MS = 200;                  

//State Tracking
unsigned long lastBuzzerTime = 0;  // Tracks when the last beep sequence fired
bool buzzerArmed = false;          // True when plant is in dry (red) zone

// Call this ONCE in setup()
void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

// Plays a short sequence of beeps
void playAlertBeeps() {
  for (int i = 0; i < NUM_BEEPS; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BEEP_DURATION_MS);
    digitalWrite(BUZZER_PIN, LOW);
    if (i < NUM_BEEPS - 1) {
      delay(BEEP_GAP_MS);
    }
  }
}

// Call this every loop() iteration.
// Passing the moisture band
//   0 = RED (critically dry) : Buzzer Starts
//   1 = YELLOW (getting dry)
//   2 = GREEN (healthy)      : Buzzer Stops
//   3 = BLUE (overwatered)
void updateBuzzer(int moistureBand) {
  if (moistureBand == 0) {
    // Plant is DRY — arm the buzzer if not already armed
    if (!buzzerArmed) {
      buzzerArmed = true;
      lastBuzzerTime = millis() - BEEP_INTERVAL; // Fire immediately on first entry
    }
    // Beep every hour while still in red zone
    unsigned long now = millis();
    if (now - lastBuzzerTime >= BEEP_INTERVAL) {
      playAlertBeeps();
      lastBuzzerTime = now;
      Serial.println("BUZZER: Dry alert fired.");
    }

  } else {
    // Plant has been watered — disarm the buzzer
    if (buzzerArmed) {
      buzzerArmed = false;
      digitalWrite(BUZZER_PIN, LOW); // Safety: make sure it's off
      Serial.println("BUZZER: Disarmed — plant watered.");
    }
  }
}