#include "actuatorControl.h"
#include "Ticker.h"
#include <WebServer.h>
#include <WiFi.h>
// #include <SPIFFS.h>

Ticker StopActuatorTicker;
WebServer server(80);

const char* ssid     = "";
const char* password = "";

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

struct ActuatorSwitch {
  // true = retracted, false = extended.
  bool state;
  
  // initial state of the actuator extended - door closed
  ActuatorSwitch() : state(false) {}

  // true calls retractActuator() opens door , false calls extendActuator() closes door
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
};

// void handleRoot() {
//   String html = R"(
// <!DOCTYPE html>
// <html lang="en">

// <head>
//   <meta charset="utf-8" />
//   <meta name="viewport" content="width=device-width, initial-scale=1.0">
//   <title>Mower Door Control</title>
//   <!-- Materialize CSS and Material Icons -->
//   <link href="https://fonts.googleapis.com/icon?family=Material+Icons" rel="stylesheet">
//   <link href="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/css/materialize.min.css" rel="stylesheet">
//   <style>
//     body {
//       display: flex;
//       flex-direction: column;
//       min-height: 100vh;
//     }

//     main {
//       flex: 1 0 auto;
//     }

//     .container {
//       margin-top: 50px;
//     }

//     /* Skip Link Styles */
//     .skip-link {
//       position: absolute;
//       left: -999px;
//       top: -999px;
//       background: #fff;
//       padding: 0.5em;
//       z-index: 1000;
//     }

//     .skip-link:focus {
//       left: 0;
//       top: 0;
//     }
//   </style>
// </head>

// <body>
//   <a href="#main-content" class="skip-link">Skip to main content</a>
//   <nav role="navigation" aria-label="Main Navigation">
//     <div class="nav-wrapper blue">
//       <a href="#" class="brand-logo center">Mower Door Actuator</a>
//     </div>
//   </nav>
//   <main id="main-content">
//     <div class="container">
//       <div class="row">
//         <!-- Retract Button Opens Door -->
//         <div class="col s12 m4">
//           <button id="retract-btn" class="waves-effect waves-light btn-large">
//             <i class="material-icons left" aria-hidden="true">arrow_upward</i>
//             Open Door
//           </button>
//         </div>

//         <div class="col s12 m4">
//         <button id="extend-btn" class="waves-effect waves-light btn-large">
//             <i class="material-icons left" aria-hidden="true">arrow_downward</i>
//              Close Door
//           </button>
//         </div>

//         <!-- Stop Button -->
//         <div class="col s12 m4">
//           <button id="stop-btn" class="waves-effect waves-light btn-large">
//             <i class="material-icons left" aria-hidden="true">pause</i>
//             Stop Actuator
//           </button>
//         </div>
//       </div>
//       <!-- Status Message Panel -->
//       <div class="row">
//         <div class="col s12">
//           <div id="status" class="card-panel teal lighten-4" role="status" aria-live="polite" style="display: none;">
//           </div>
//         </div>
//       </div>
//     </div>
//   </main>
//   <!-- JavaScript to handle SPA interactions -->
//   <script>
//     // Helper function to display status messages
//     function showStatus(message) {
//       const statusDiv = document.getElementById("status");
//       statusDiv.style.display = "block";
//       statusDiv.textContent = message;
//       setTimeout(() => { statusDiv.style.display = "none"; }, 5000);
//     }

//     // Send command using the Fetch API
//     function sendCommand(endpoint) {
//       fetch(endpoint)
//         .then(response => response.text())
//         .then(text => showStatus(text))
//         .catch(error => showStatus("Error: " + error));
//     }

//     // Event listeners for buttons
//     document.getElementById("extend-btn").addEventListener("click", () => {
//       sendCommand("/extend");
//     });
//     document.getElementById("retract-btn").addEventListener("click", () => {
//       sendCommand("/retract");
//     });
//     document.getElementById("stop-btn").addEventListener("click", () => {
//       sendCommand("/stop");
//     });
//   </script>
//   <!-- Materialize JS -->
//   <script src="https://cdnjs.cloudflare.com/ajax/libs/materialize/1.0.0/js/materialize.min.js"></script>
// </body>

// </html>
//   )";
//   server.send(200, "text/html", html);
// }

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

    /* Mobile-specific styles: stack buttons vertically and center them */
    @media only screen and (max-width: 600px) {
      .row {
        display: flex;
        flex-direction: column;
        align-items: center;
      }
      .row > div {
        width: 100%;
        display: flex;
        justify-content: center;
        margin-bottom: 15px;
      }
    }
  </style>
</head>

<body>
  <a href="#main-content" class="skip-link">Skip to main content</a>
  <nav role="navigation" aria-label="Main Navigation">
    <div class="nav-wrapper blue">
      <a href="#" class="brand-logo center">Mower Door Actuator</a>
    </div>
  </nav>
  <main id="main-content">
    <div class="container">
      <div class="row">
        <!-- Retract Button Opens Door -->
        <div class="col s12 m4">
          <button id="retract-btn" class="waves-effect green darken-1 btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_upward</i>
            Open Door
          </button>
        </div>

        <div class="col s12 m4">
          <button id="extend-btn" class="waves-effect orange darken-1 btn-large">
            <i class="material-icons left" aria-hidden="true">arrow_downward</i>
            Shut Door
          </button>
        </div>
        
        <!-- Stop Button -->
        <div class="col s12 m4">
          <button id="stop-btn" class="waves-effect red darken-1
 btn-large">
            <i class="material-icons left" aria-hidden="true">pause</i>
            Stop Door
          </button>
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
  Serial.println("Setup :");
  connectToWiFi();
  setupActuator();
  server.on("/", handleRoot);
  server.on("/extend", handleExtend);
  server.on("/retract", handleRetract);
  server.on("/stop", handleStop);
  server.begin();
  Serial.println("Web server started!");
  new ActuatorSwitch();
}

void loop() {
  server.handleClient();
}
