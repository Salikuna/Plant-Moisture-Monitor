#include <Arduino.h>
#include "buzzer.h"

// Pin Definition
const int BUZZER_PIN = 17;

// Timing
const unsigned long BEEP_INTERVAL = 3600000UL;
const int BEEP_DURATION_MS = 500;
const int NUM_BEEPS = 3;
const int BEEP_GAP_MS = 200;

// State Tracking
unsigned long lastBuzzerTime = 0;
bool buzzerArmed = false;

void setupBuzzer() {
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN); // clean start
}

void playAlertBeeps() {
  for (int i = 0; i < NUM_BEEPS; i++) {
    tone(BUZZER_PIN, 2000, BEEP_DURATION_MS); // 2000Hz = clear urgent beep
    delay(BEEP_DURATION_MS + BEEP_GAP_MS);    // wait for tone to finish + gap
  }
  noTone(BUZZER_PIN); // stop cleanly after beeps
}

void updateBuzzer(int moistureBand) {
  if (moistureBand == 0) {
    if (!buzzerArmed) {
      buzzerArmed = true;
      lastBuzzerTime = millis() - BEEP_INTERVAL; // fire immediately first time
    }
    unsigned long now = millis();
    if (now - lastBuzzerTime >= BEEP_INTERVAL) {
      playAlertBeeps();
      lastBuzzerTime = now;
      Serial.println("BUZZER: Dry alert fired.");
    }
  } else {
    if (buzzerArmed) {
      buzzerArmed = false;
      noTone(BUZZER_PIN); // safety stop
      Serial.println("BUZZER: Disarmed — plant watered.");
    }
  }
}