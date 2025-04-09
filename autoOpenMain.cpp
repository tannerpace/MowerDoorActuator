#include "actuatorControl.h"
#include "Ticker.h"
#include <WebServer.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Global variable declarations
Ticker StopActuatorTicker;
WebServer server(80);

const char* ssid     = "";
const char* password = "";
bool autoOpenState = false;
const String targetMac = ""; // Target Bluetooth MAC address
volatile bool bleDetected = false;            // Flag set when target is detected
BLEScan* pBLEScan = nullptr;

// Function prototypes
void toggleAutoOpenState();
void connectToWiFi();
void handleExtend();
void handleRetract(); 
void handleStop();

// Structure for actuator switch
struct ActuatorSwitch {
  // true = retracted, false = extended.
  bool state;
  
  // initial state of the actuator (extended – door closed)
  ActuatorSwitch() : state(false) {}

  // true calls retractActuator() (opens door), false calls extendActuator() (closes door)
  void update(bool newState) {
    StopActuatorTicker.detach();
    if (newState) {
      retractActuator();
      StopActuatorTicker.once_ms(90 * 1000, stopActuator);
    } else {
      extendActuator();
      StopActuatorTicker.once_ms(90 * 1000, stopActuator);
    }
    state = newState;
  }
  getState() {
    return state;
  }
};

// BLE Advertised Device Callback Class
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) override {
    String advMac = advertisedDevice.getAddress().toString().c_str();
    Serial.print("Found BLE Device: ");
    Serial.println(advMac);
    // If autoopen is enabled and the found device matches the target MAC,
    // set the flag for triggering the retract action.
    if (autoOpenState && advMac.equalsIgnoreCase(targetMac)) {
      Serial.println("Target device found while Auto Open enabled. Calling handleRetract()");
      bleDetected = true;
    }
  }
};

void toggleAutoOpenState() {
  autoOpenState = !autoOpenState;
  Serial.print("Auto Open State: ");
  Serial.println(autoOpenState ? "Enabled" : "Disabled");
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Door Control</title>
  <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
  <link href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css" rel="stylesheet">
  <style>
    body { display: flex; flex-direction: column; min-height: 100vh; }
    main { flex: 1 0 auto; }
    .container { margin-top: 50px; }
    .skip-link { position: absolute; left: -999px; top: -999px; background: #fff; padding: 0.5em; z-index: 1000; }
    .skip-link:focus { left: 0; top: 0; }
    @media only screen and (max-width: 600px) {
      .row { display: flex; flex-direction: column; align-items: center; }
      .row > div { width: 100%; display: flex; justify-content: center; margin-bottom: 15px; }
    }
  </style>
</head>
<body>
  <a href="#main-content" class="skip-link">Skip to main content</a>
  <nav role="navigation" aria-label="Main Navigation">
    <div class="nav-wrapper blue">
      <a href="#" class="brand-logo center">Door</a>
    </div>
  </nav>
  <main id="main-content">
    <div class="container">
      <div class="row">
        <!-- Retract Button Opens Door -->
        <div class="col s12 m4">
          <button id="retract-btn" class="waves-effect green darken-1 btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_upward</i>
            Open
          </button>
        </div>
        <div class="col s12 m4">
          <button id="extend-btn" class="waves-effect orange darken-1 btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_downward</i>
            Shut
          </button>
        </div>
        <!-- Stop Button -->
        <div class="col s12 m4">
          <button id="stop-btn" class="waves-effect red darken-1 btn-large">
            <i class="material-icons left" aria-hidden="true">pause</i>
            Stop
          </button>
        </div>
      </div>
      <!-- Checkbox to change autoopen state -->
      <div class="row">
        <div class="col s12 m4 offset-m4">
          <label>
            <input type="checkbox" id="autoopen" />
            <span>Auto Open</span>
          </label>
        </div>
      </div>
      <!-- Status Message Panel -->
      <div class="row">
        <div class="col s12">
          <div id="status" class="card-panel teal lighten-4" role="status" aria-live="polite" style="display: none;">
          </div>
        </div>
      </div>
    </div>
  </main>
  <script>
    function showStatus(message) {
      const statusDiv = document.getElementById("status");
      statusDiv.style.display = "block";
      statusDiv.textContent = message;
      setTimeout(() => { statusDiv.style.display = "none"; }, 5000);
    }
    function sendCommand(endpoint) {
      fetch(endpoint)
        .then(response => response.text())
        .then(text => showStatus(text))
        .catch(error => showStatus("Error: " + error));
    }
    document.getElementById("extend-btn").addEventListener("click", () => {
      sendCommand("/extend");
    });
    document.getElementById("retract-btn").addEventListener("click", () => {
      sendCommand("/retract");
    });
    document.getElementById("stop-btn").addEventListener("click", () => {
      sendCommand("/stop");
    });
    document.getElementById("autoopen").addEventListener("change", (event) => {
      const state = event.target.checked ? "enabled" : "disabled";
      sendCommand("/autoopen/" + state);
    });
  </script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js"></script>
</body>
</html>
  )";
  server.send(200, "text/html", html);
}

void handleExtend() {
  StopActuatorTicker.detach(); 
  extendActuator();
  StopActuatorTicker.once_ms(90 * 1000, stopActuator);
  server.send(200, "text/plain", "Actuator extending...");
}

void handleRetract() {
  StopActuatorTicker.detach();
  retractActuator();
  StopActuatorTicker.once_ms(90 * 1000, stopActuator);
  server.send(200, "text/plain", "Actuator retracting...");
}

void handleStop() {
  StopActuatorTicker.detach();
  stopActuator();
  server.send(200, "text/plain", "Actuator stopped.");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Setup:");
  connectToWiFi();
  setupActuator();

  // Initialize BLE and start continuous scanning.
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  pBLEScan->start(0); // 0 means continuous scanning
  
  // Set up web server endpoints.
  server.on("/", handleRoot);
  server.on("/extend", handleExtend);
  server.on("/retract", handleRetract);
  server.on("/stop", handleStop);
  // Endpoints to toggle auto open state.
  server.on("/autoopen/enabled", [](){
    autoOpenState = true;
    Serial.println("Auto Open State: Enabled");
    server.send(200, "text/plain", "Auto Open Enabled");
  });
  server.on("/autoopen/disabled", [](){
    autoOpenState = false;
    Serial.println("Auto Open State: Disabled");
    server.send(200, "text/plain", "Auto Open Disabled");
  });
  server.begin();
  Serial.println("Web server started!");
  
  // Initialize actuator switch 
  new ActuatorSwitch();
}

void loop() {
  server.handleClient();

  // Check if BLE scanning detected the target device while auto open is enabled.
  if (bleDetected) {
    bleDetected = false; // Reset the flag
  // if its already retracted, do nothing
  if (actuatorSwitch.getState() == true) {
    Serial.println("Actuator already retracted, no action taken.");
  } else {
     // Call the function to handle the retract action
    handleRetract();
  }
}
