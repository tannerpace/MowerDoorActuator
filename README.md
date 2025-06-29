# Mower Door Actuator

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
  - **Select your board:** Choose the ESP32-C6 entry



## Code Overview

The code handles:
- **Wi-Fi Connection:** The device connects to your specified Wi-Fi network.
- **Web Server:**  
  - Serves a simple GUI to send commands for extending, retracting, or stopping the actuator.
  - Uses Materialize CSS for a responsive design.
- **Actuator Control:**  
  - `extendActuator()`, `retractActuator()`, and `stopActuator()` manage relay outputs.
  - A `Ticker` is used to automatically turn off power to the relay after 90 seconds, the actuator is one that has built in overdrive control but this is just a precaution.
- **HomeSpan Accessory:**  
  - Exposes the actuator as a HomeKit accessory with a switch interface.
  - Pairing is set with the code `83722189`.

I later removed the HomeKit dependency and am serving a web UI using vanilla JavaScript and HTML, I saved the website directly under my phone and interact with them overdoor with just a few clicks. 

## Usage

1. **Compile and Upload:**  
   - Open the project in the Arduino IDE.
   - Verify and upload the code to your ESP32-C6 board. 
   
2. **Connect to Wi-Fi:**  
   - The board will connect to your Wi-Fi network. Find the ip address by accessing the admin panel of your router, note the new devices Look for the new ip address. 


Finding Your Device’s IP Address

To access the web interface, you’ll need to know the ESP32’s IP address on your local network. The most reliable way is to scan your network:

📡 Scan for IP Addresses

Mac (Terminal):

sudo nmap -sn 192.168.1.0/24

Replace 192.168.1.0/24 with your network’s subnet if different.
You can find your subnet by running ifconfig and looking for your local IP.

Windows (PowerShell or Command Prompt):

1. Install Nmap.


2. Run:

nmap -sn 192.168.1.0/24

   
3. **Access the Web Interface:**  
   - Open a web browser and navigate to `http://<BOARD_IP_ADDRESS>/`.
   - verify the page is accessible, i perfer homeassistant to control the actuator, however it is also working on local network at the page.
    
### Partitioning

When using the Arduino IDE, set your partition scheme via **Tools → Partition Scheme**:
- select a scheme 
 **"Minimal (2mb NO OTA partition)"**   2 MB per partition.


**Connections:**

1. **NOYITO Power Supply**:
   - Input:
     - Connect AC 120V (hot line) and neutral to input terminals.
   - Output:
     - Connect DC +12V (positive) to relay module VCC.
     - Connect DC GND (negative) to relay module GND and ESP32 GND (Green wire).

2. **ESP32 DevKitC-1**:
   - 5V Pin (Yellow wire) to 5V power input on Relay Module.
   - GND Pin (Green wire) to common ground (Power supply and relay GND).
   - GPIO Pin 18 (Blue wire) to Relay IN1.
   - GPIO Pin 19 (Purple wire) to Relay IN2.

3. **2-Channel Relay Module**:
   - Connect Relay outputs:
     - Relay Channel 1 NO (normally open) to Actuator Positive terminal.
     - Relay Channel 2 NO (normally open) to Actuator Negative terminal.
     - Both Relay Common (COM) terminals connected to +12V and GND respectively from power supply.

4. **VEVOR Linear Actuator**:
   - Positive and Negative terminals connected to relay output as described above.

**Color Coding:**
- Green: Ground
- Yellow: +5V DC
- Blue: GPIO Pin 18
- Purple: GPIO Pin 19
- Red: +12V DC
- Black: DC Negative / GND

Ensure the connections are properly insulated and secured. Mount all components within a protective enclosure to avoid moisture and dust ingress.



