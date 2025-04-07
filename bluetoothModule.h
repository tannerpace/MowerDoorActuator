#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Global flag for auto open mode declared elsewhere
extern bool autoOpenActive;

// Initializes Bluetooth scanning with custom callbacks.
void initBluetooth();

#endif // BLUETOOTHMODULE_H