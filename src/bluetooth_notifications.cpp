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

void sendStatusChangeAlert(int moistureBand, int moisturePercent) {
  if (!SerialBT.hasClient()) {
    return;
  }

  SerialBT.print("Plant status changed: ");

  if (moistureBand == 0) {
    SerialBT.print("RED - Plant needs water urgently. ");
  }
  else if (moistureBand == 1) {
    SerialBT.print("YELLOW - Plant needs water soon. ");
  }
  else if (moistureBand == 2) {
    SerialBT.print("GREEN - Plant has enough water. ");
  }
  else if (moistureBand == 3) {
    SerialBT.print("BLUE - Plant has too much water. ");
  }

  SerialBT.print("Moisture: ");
  SerialBT.print(moisturePercent);
  SerialBT.println("%");
}