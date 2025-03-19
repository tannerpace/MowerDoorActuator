# Mower Door Actuator

This repository contains the code for controlling a linear actuator (the "Mower Door Actuator") using an ESP32-C6 development board. The actuator is controlled via both a web GUI and HomeKit (using [HomeSpan](https://github.com/HomeSpan/HomeSpan)). The system automatically stops the actuator after 90 seconds to prevent over-driving.

## Features

- **Wi-Fi Connectivity:** Connects to your local Wi-Fi network.
- **Web Interface:** A Materialize CSS–based GUI for extending, retracting, or stopping the actuator.
- **HomeKit Integration:** Uses HomeSpan to integrate with Apple HomeKit.
- **Auto Stop:** Automatically stops the actuator after 90 seconds.
- **Relay Control:** Uses two relay channels to manage actuator direction.

## Hardware List

- **ESP32-C6-DevKitC-1**  
  *Espressif Systems Development Board*
- **NOYITO AC to DC 12V 8A Isolated Power Supply Module**  
  Converts AC 120V (110-260V) to DC 12V 8A (54W, peak 8A) with overcurrent, overload, and short-circuit protection.
- **2 Channel DC 5V Relay Module with Optocoupler Relay**
- **VEVOR Linear Actuator 12V, 12 Inch High Load 330lbs/1500N Linear Actuator**  
  Includes a mounting bracket and has IP54 protection.

## Software Environment & Setup

### Arduino IDE / PlatformIO

- **Arduino IDE:**  
  - Install the latest version from the [Arduino website](https://www.arduino.cc/en/software).
  - In **Tools → Board → Boards Manager**, install **"ESP32 by Espressif Systems"**.  
  - **Select your board:** Choose the ESP32-C6 entry (if available). If not, check Espressif’s documentation or GitHub for development branch support.
  
- **PlatformIO (VS Code):**  
  - Install the PlatformIO extension.
  - Create an environment in your `platformio.ini` similar to:
    ```ini
    [env:esp32c6dev]
    platform = espressif32
    board = esp32c6-devkitc-1
    framework = arduino
'''
### Arduino IDE is much easier to set up than the espressif vscode extension 
    
### Partitioning

When using the Arduino IDE, set your partition scheme via **Tools → Partition Scheme**:
- If you plan to use OTA updates or need two partitions, select a scheme like **"Minimal (2 OTA partitions)"** or another scheme that provides two partitions.

### Libraries

- **HomeSpan:**  
  - Install via the Arduino Library Manager or clone from [GitHub](https://github.com/HomeSpan/HomeSpan).
- **Ticker Library:**  
  - Also available through the Arduino Library Manager.

### Environment Setup

1. **Wi-Fi Credentials:**  
   Update the `ssid` and `password` variables in your code with your Wi-Fi network details.
   
2. **Relay Pins:**  
   The code uses `GPIO18` (RELAY1_PIN) and `GPIO19` (RELAY2_PIN) to control the actuator.
   
3. **Serial Monitor:**  
   Open the Serial Monitor at a baud rate of **115200** to view debug messages (e.g., IP address assignment and actuator actions).

## Code Overview

The code handles:
- **Wi-Fi Connection:** The device connects to your specified Wi-Fi network.
- **Web Server:**  
  - Serves a simple GUI to send commands for extending, retracting, or stopping the actuator.
  - Uses Materialize CSS for a responsive design.
- **Actuator Control:**  
  - `extendActuator()`, `retractActuator()`, and `stopActuator()` manage relay outputs.
  - A `Ticker` is used to automatically stop the actuator after 90 seconds.
- **HomeSpan Accessory:**  
  - Exposes the actuator as a HomeKit accessory with a switch interface.
  - Pairing is set with the code `83722189`.

## Usage

1. **Compile and Upload:**  
   - Open the project in the Arduino IDE or PlatformIO.
   - Verify and upload the code to your ESP32-C6 board.
   
2. **Connect to Wi-Fi:**  
   - The board will connect to your Wi-Fi network. Check the Serial Monitor for the assigned IP address.
   
3. **Access the Web Interface:**  
   - Open a web browser and navigate to `http://<BOARD_IP_ADDRESS>/`.
   - Use the provided buttons to extend, retract, or stop the actuator.
   
4. **HomeKit Integration:**  
   - Pair the device with your HomeKit app using the pairing code set in the code.
   - Control the actuator using HomeKit as an accessory named "Mower Door".

## Troubleshooting

- **Wi-Fi Connection Issues:**  
  Ensure your Wi-Fi credentials are correct and that the board supports 2.4 GHz networks.
  
- **HomeKit Pairing Problems:**  
  Verify that your iOS device is on the same network as the ESP32-C6 and that the pairing code matches.
  
- **Free Space Errors:**  
  If you encounter memory issues, try a different partition scheme with more space.
  

## License
MIT
This project is provided as-is without any warranty. 

---
