#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Smart Plant Monitor started.");
}

void loop() {
  Serial.println("Testing main loop...");
  delay(2000);
}
