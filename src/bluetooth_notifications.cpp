#include <BluetoothSerial.h>
#include "bluetooth.h"

BluetoothSerial SerialBT;

const unsigned long STATUS_INTERVAL = 15UL * 60UL * 1000UL;
unsigned long lastStatusTime = 0;
bool wasConnected = false;

void beginBluetooth() {
    SerialBT.begin("PlantMonitor");
}

void sendRedZoneAlert() {
    if (SerialBT.hasClient()) {
        SerialBT.println("ALERT: Plant needs water urgently!");
    }
}

void sendStatusUpdate(int moisturePercent, unsigned long currentTime) {
    bool isConnected = SerialBT.hasClient();

    if (isConnected && !wasConnected) {
        SerialBT.println("Connected to PlantMonitor.");
        SerialBT.print("Current moisture level: ");
        SerialBT.print(moisturePercent);
        SerialBT.println("%");

        lastStatusTime = currentTime;
    }
    
    if (isConnected && currentTime - lastStatusTime >= STATUS_INTERVAL) {
        lastStatusTime = currentTime;

        SerialBT.print("Status update: Moisture is ");
        SerialBT.print(moisturePercent);
        SerialBT.println("%");
    }
    wasConnected = isConnected;
}