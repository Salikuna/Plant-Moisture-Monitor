#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
const unsigned long STATUS_INTERVAL = 30UL * 60UL * 1000UL;
unsigned long lastStatusTime = 0;

void setup(){
    SerialBT.begin("PlantMonitor");
}

void loop(){
    if (moisturePercent <= 25){
        digitalWrite(RED_LED_PIN, HIGH);
        Serial.println("Status: RED - Plant needs water urgently.");
        SerialBT.println("ALERT: Plant needs water urgently!")
    }

    if (currentTime - lastStatusTime >= STATUS_INTERVAL){
        lastStatusTime = currentTime;
        SerialBT.print("Status update: Moisture is");
        SerialBT.print(moisturePercent);
        SerialBT.println("%");
    }
}