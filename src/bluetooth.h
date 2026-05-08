#ifndef BLUETOOTH_NOTIFICATIONS_H
#define BLUETOOTH_NOTIFICATIONS_H

void beginBluetooth();
void sendRedZoneAlert();
void sendStatusUpdate(int moisturePercent, unsigned long currentTime);
void sendStatusChangeAlert(int moistureBand, int moisturePercent);

#endif