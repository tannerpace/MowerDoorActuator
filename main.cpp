#include "HomeSpan.h"
#include "WiFi.h"
#include "Ticker.h"
#include <WebServer.h>
#include <ESPmDNS.h>

Ticker StopActuatorTicker;
WebServer server(80);

const char* ssid     = "redact";
const char* password = "redact";

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

// Actuator control functions
#define RELAY1_PIN 18
#define RELAY2_PIN 19

void extendActuator() {
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, HIGH);
}

void retractActuator() {
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, LOW);
}

void stopActuator() {
  digitalWrite(RELAY1_PIN, HIGH);
  digitalWrite(RELAY2_PIN, HIGH);
}

// --- HomeSpan Accessory ---
struct ActuatorSwitch : Service::Switch {
  SpanCharacteristic *power;
  ActuatorSwitch() : Service::Switch() {
    power = new Characteristic::On();
  }
  boolean update() override {
    StopActuatorTicker.detach(); // Cancel any previously scheduled stopActuator call
    if (power->getNewVal()) {
      retractActuator();
      StopActuatorTicker.once_ms(90 * 1000, stopActuator);
    } else {
      extendActuator();
      StopActuatorTicker.once_ms(90 * 1000, stopActuator);
    }
    return true;
  }
};

// --- Web Server Handlers ---
// Root endpoint serving the updated SPA UI with improved accessibility
void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Mower Door Control</title>
  <!-- Materialize CSS and Material Icons -->
  <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
  <link href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css" rel="stylesheet">
  <style>
    body {
      display: flex;
      flex-direction: column;
      min-height: 100vh;
    }
    main {
      flex: 1 0 auto;
    }
    .container {
      margin-top: 50px;
    }
    /* Skip Link Styles */
    .skip-link {
      position: absolute;
      left: -999px;
      top: -999px;
      background: #fff;
      padding: 0.5em;
      z-index: 1000;
    }
    .skip-link:focus {
      left: 0;
      top: 0;
    }
  </style>
</head>
<body>
  <!-- Skip to main content link -->
  <a href="#main-content" class="skip-link">Skip to main content</a>
  
  <!-- Navigation Bar -->
  <nav role="navigation" aria-label="Main Navigation">
    <div class="nav-wrapper blue">
      <a href="#" class="brand-logo center">Mower Door Actuator</a>
    </div>
  </nav>
  <main id="main-content">
    <div class="container">
      <div class="row">
        <!-- Extend Button -->
        <div class="col s12 m4">
          <button id="extend-btn" class="waves-effect waves-light btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_downward</i>
            Close Door
          </button>
        </div>
        <!-- Retract Button -->
        <div class="col s12 m4">
          <button id="retract-btn" class="waves-effect waves-light btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_upward</i>
            Open Door
          </button>
        </div>
        <!-- Stop Button -->
        <div class="col s12 m4">
          <button id="stop-btn" class="waves-effect waves-light btn-large">
            <i class="material-icons left" aria-hidden="true">pause</i>
            Stop Actuator
          </button>
        </div>
      </div>
      <!-- Status Message Panel -->
      <div class="row">
        <div class="col s12">
          <div id="status" class="card-panel teal lighten-4" role="status" aria-live="polite" style="display: none;"></div>
        </div>
      </div>
    </div>
  </main>
  <!-- JavaScript to handle SPA interactions -->
  <script>
    // Helper function to display status messages
    function showStatus(message) {
      const statusDiv = document.getElementById("status");
      statusDiv.style.display = "block";
      statusDiv.textContent = message;
      setTimeout(() => { statusDiv.style.display = "none"; }, 5000);
    }

    // Send command using the Fetch API
    function sendCommand(endpoint) {
      fetch(endpoint)
        .then(response => response.text())
        .then(text => showStatus(text))
        .catch(error => showStatus("Error: " + error));
    }

    // Event listeners for buttons
    document.getElementById("extend-btn").addEventListener("click", () => {
      sendCommand("/extend");
    });
    document.getElementById("retract-btn").addEventListener("click", () => {
      sendCommand("/retract");
    });
    document.getElementById("stop-btn").addEventListener("click", () => {
      sendCommand("/stop");
    });
  </script>
  <!-- Materialize JS -->
  <script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js"></script>
</body>
</html>
  )";

  server.send(200, "text/html", html);
}

// Extend Actuator endpoint
void handleExtend() {
  StopActuatorTicker.detach(); 
  extendActuator();
  StopActuatorTicker.once_ms(90 * 1000, stopActuator);
  server.send(200, "text/plain", "Actuator extending...");
}

// Retract Actuator endpoint
void handleRetract() {
  StopActuatorTicker.detach();
  retractActuator();
  StopActuatorTicker.once_ms(90 * 1000, stopActuator);
  server.send(200, "text/plain", "Actuator retracting...");
}

// Stop Actuator endpoint
void handleStop() {
  StopActuatorTicker.detach();
  stopActuator();
  server.send(200, "text/plain", "Actuator stopped.");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Hello from setup!");

  // Connect to WiFi
  connectToWiFi();

  // Start mDNS with the chosen hostname
  if (!MDNS.begin("mowerdoor")) {
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http", "tcp", 80);

  // Initialize Web Server Endpoints
  server.on("/", handleRoot);
  server.on("/extend", handleExtend);
  server.on("/retract", handleRetract);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("Web server started!");

  // HomeSpan Setup
  homeSpan.setPairingCode("83722189");
  homeSpan.begin(Category::GarageDoorOpeners, "Mower Door");

  // Relay pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  stopActuator();

  // HomeSpan Accessory
  new SpanAccessory();
  new Service::AccessoryInformation();
  new Characteristic::Identify();
  new Characteristic::Manufacturer("TPB");
  new Characteristic::Model("MowerDoorActuator");
  new Characteristic::SerialNumber("12345678");
  new ActuatorSwitch();
}

void loop() {
  // Handle HomeSpan and WebServer
  homeSpan.poll();
  server.handleClient();
}
