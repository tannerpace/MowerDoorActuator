# Mower Door Actuator

This repository contains the code for controlling a linear actuator (the "Mower Door Actuator") using an ESP32-C6 development board. The actuator is controlled via both a web GUI and HomeKit (using [HomeSpan](https://github.com/HomeSpan/HomeSpan)). The system automatically turns off the relay after 90 seconds to prevent wasting power. The actuator has a built-in stop so no need to worry about overdriving.

## Features

- **Wi-Fi Connectivity:** Connects to your local Wi-Fi network.
- **Web Interface:** A Materialize CSS–based GUI for extending, retracting, or stopping the actuator.
- **HomeKit Integration:** Uses HomeSpan to integrate with Home assistant or Apple HomeKit.
- **Auto Stop:** The Ticker library is used to automatically stop the actuator after 90 seconds, which in turn shuts down the relay. This functionality helps save power by ensuring that the relay is not left on indefinitely.
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
    
### Partitioning

When using the Arduino IDE, set your partition scheme via **Tools → Partition Scheme**:
- I selected a scheme like **"Minimal (2  NO OTA partition)"**  otherwise you won't have enough space for the entire Arduino sketch. 

### Libraries

- **HomeSpan:**  
  - Install via the Arduino Library Manager or clone from [GitHub](https://github.com/HomeSpan/HomeSpan).
- **Ticker Library:**  
  - Also available through the Arduino Library Manager. This one is optional but recommended it handles turning off the relay power after 90 seconds to conserve electricity.

### To use just plug in your Wi-Fi credentials. Then flash the code to your esc-32. 

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
   - Open the project in the Arduino IDE.
   - Verify and upload the code to your ESP32-C6 board. 
   
2. **Connect to Wi-Fi:**  
   - The board will connect to your Wi-Fi network. Find the ip address by accessing the admin panel of your router, note the new devices ip address. if you are not sure of which one the new devices simply unplug power to the esc32 refresh your admin page and noting the device set was removed. 
   
3. **Access the Web Interface:**  
   - Open a web browser and navigate to `http://<BOARD_IP_ADDRESS>/`.
   - Since I'm using home assistant i didn't test the functioning this part but did verify the page was accessible.
   

## Hardware List

- **ESP32-C6-DevKitC-1**  
  *Espressif Systems Development Board* this was found on AliExpress
- **NOYITO AC to DC 12V 8A Isolated Power Supply Module**  
  Converts AC 120V (110-260V) to DC 12V 8A (54W, peak 8A) with overcurrent, overload, and short-circuit protection. available on Amazon
- **2 Channel DC 5V Relay Module with Optocoupler Relay** available on Amazon
- **VEVOR Linear Actuator 12V, 12 Inch High Load 330lbs/1500N Linear Actuator**  also available on Amazon
  Includes a mounting bracket and has IP54 protection.

## Software Environment & Setup

### Arduino IDE / PlatformIO

- **Arduino IDE:**  
  - Install the latest version from the [Arduino website](https://www.arduino.cc/en/software).
  - In **Tools → Board → Boards Manager**, install **"ESP32 by Espressif Systems"**.  
  - **Select your board:** Choose the ESP32-C6 entry (if available). If not, check Espressif’s documentation or GitHub for development branch support.
    
### Partitioning

When using the Arduino IDE, set your partition scheme via **Tools → Partition Scheme**:
- I selected a scheme like **"Minimal (2mb NO OTA partition)"**  otherwise you won't have enough space for the entire Arduino sketch.  I'm filling this out for memory, but I think it was  2 MB per partition.

 relay outputs.
 pairing code matches.
  
- **Free Space Errors:**  
  If you encounter memory issues, try a different partition scheme with more space.
  

## License
MIT
This project is provided as-is without any warranty.  This project was inspired by a Reddit thread in /r Segwaynavimow

---
