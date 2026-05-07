#include <BluetoothSerial.h>
#include "bluetooth_notifications.h"

BluetoothSerial SerialBT;

const unsigned long STATUS_INTERVAL = 30UL * 60UL * 1000UL;
unsigned long lastStatusTime = 0;

void beginBluetooth() {
    SerialBT.begin("PlantMonitor");
}

void sendRedZoneAlert() {
    SerialBT.println("ALERT: Plant needs water urgently!");
}

void sendStatusUpdate(int moisturePercent, unsigned long currentTime) {
    if (currentTime - lastStatusTime >= STATUS_INTERVAL) {
        lastStatusTime = currentTime;

        SerialBT.print("Status update: Moisture is ");
        SerialBT.print(moisturePercent);
        SerialBT.println("%");
    }
}