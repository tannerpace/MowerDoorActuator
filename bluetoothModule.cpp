#include "bluetoothModule.h"
#include "actuatorControl.h" // For retractActuator() and stopActuator()
#include "Ticker.h"

// Externally declared ticker from main file
extern Ticker StopActuatorTicker;

// Hardcoded target BLE device MAC address (update with actual value)
static const std::string targetMac = "AA:BB:CC:DD:EE:FF";

// Custom BLE callback to detect advertised devices
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    // Compare detected device's MAC address with the target MAC
    if (advertisedDevice.getAddress().toString() == targetMac) {
      Serial.println("Target BLE device detected!");
      // If auto open mode is enabled, retract the actuator
      if (autoOpenActive) {
        StopActuatorTicker.detach();
        retractActuator();
        StopActuatorTicker.once_ms(90 * 1000, stopActuator);
        Serial.println("Actuator retracted due to BLE detection.");
      }
    }
  }
};

void initBluetooth() {
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  // Start scanning indefinitely (0 duration means continuous scan)
  pBLEScan->start(0, false);
}