#include "HomeSpan.h"
#include "WiFi.h"
#include "Ticker.h"
#include <WebServer.h>  


Ticker StopActuatorTicker;
WebServer server(80); 


const char* ssid     = "redacted";
const char* password = "redacted";


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
// Root endpoint
// Displays the control interface
// Uses Materialize CSS for styling
// Provides buttons to extend, retract, and stop the actuator
void handleRoot() {
  String html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Mower Door Control</title>
  <meta charset="utf-8" />
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
  </style>
</head>
<body>
  <!-- Navigation Bar -->
  <nav>
    <div class="nav-wrapper blue">
      <a href="#" class="brand-logo center">Mower Door Actuator</a>
    </div>
  </nav>
  <main>
    <div class="container">
      <div class="row">
        <!-- Extend Button -->
        <div class="col s12 m4">
          <a href="/extend" class="waves-effect waves-light btn-large">
            <i class="material-icons left">arrow_upward</i>
            Extend Actuator
          </a>
        </div>
        <!-- Retract Button -->
        <div class="col s12 m4">
          <a href="/retract" class="waves-effect waves-light btn-large">
            <i class="material-icons left">arrow_downward</i>
            Retract Actuator
          </a>
        </div>
        <!-- Stop Button -->
        <div class="col s12 m4">
          <a href="/stop" class="waves-effect waves-light btn-large">
            <i class="material-icons left">pause</i>
            Stop Actuator
          </a>
        </div>
      </div>
    </div>
  </main>
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

  // WiFi + WebServer
  connectToWiFi();
  server.on("/", handleRoot);
  server.on("/extend", handleExtend);
  server.on("/retract", handleRetract);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("Web server started!");

  // HomeSpan
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
