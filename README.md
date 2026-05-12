# Vehicle-to-Vehicle-Communication-System

# Vehicle-to-Vehicle (V2V) Communication System

## Overview

This project is an ESP32-based Vehicle-to-Vehicle (V2V) communication prototype developed to improve road safety through real-time wireless communication between nearby vehicles.

The system enables transmission of emergency alerts, brake signals, turn indications, and overtaking notifications using ESP-NOW communication protocol.

The prototype also includes OLED-based visual alerts, buzzer indications, LED notifications, and approximate vehicle distance estimation using RSSI signal strength.

---

## Objectives

- Improve road safety using wireless communication
- Enable real-time alert transmission between vehicles
- Demonstrate low-latency ESP-NOW communication
- Provide driver warning indications using visual and audio alerts
- Develop a compact embedded-system-based V2V prototype

---

## Features

- Real-time vehicle-to-vehicle communication
- Emergency alert transmission
- Brake warning system
- Left and right turn indication alerts
- Overtake indication alerts
- OLED display animations and notifications
- RSSI-based approximate distance estimation
- Buzzer alert system for emergency situations
- LED-based alert indicators
- ESP-NOW low-power wireless communication

---

## Technologies Used

### Hardware
- ESP32 Development Board
- OLED Display (SSD1306)
- LEDs
- Buzzer
- Push Buttons
- Jumper Wires

### Software
- Arduino IDE
- Embedded C/C++
- ESP-NOW Protocol
- Adafruit SSD1306 Library
- WiFi Library

---

## System Architecture

```plaintext
[ Transmitter Unit ]
      |
      |  ESP-NOW Wireless Communication
      v
[ Receiver Unit ]
      |
      +--> OLED Alert Display
      +--> LED Indicators
      +--> Buzzer Alerts
```

---

## Alert Priority System

| Priority | Alert Type |
|----------|-------------|
| 1 | Emergency |
| 2 | Brake |
| 3 | Right Turn |
| 4 | Left Turn |
| 5 | Overtake |

---

## Project Structure

```plaintext
hardware/   -> Circuit diagrams and hardware documentation
software/   -> Source code for transmitter and receiver
images/     -> Prototype images and screenshots
docs/       -> Reports, presentations, and diagrams
demo/       -> Demo video links
```

---

## Current Status

- Repository initialized
- Project documentation completed
- Source code upload in progress
- Hardware documentation pending
- Prototype images pending

---

## Future Enhancements

- GPS-based vehicle tracking
- Mobile application integration
- AI-based collision prediction
- Cloud connectivity
- Real-time traffic monitoring
- Advanced vehicle analytics dashboard

---

## Applications

- Smart transportation systems
- Road safety systems
- Accident prevention systems
- Intelligent vehicle communication
- IoT-based automotive solutions

---

## Author

Nitish T  
EEE Student  
