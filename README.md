# Dashboard Design Using CAN Bus

# Overview
Dashboard Design Using CAN Bus is a distributed embedded system developed using LPC2129 ARM7 microcontrollers and the Controller Area Network (CAN) protocol. The project simulates an automotive dashboard where multiple Electronic Control Units (ECUs) communicate over a CAN network to exchange real-time vehicle information.
The system consists of three independent CAN nodes:
* Main Node
* Fuel Node
* Indicator Node
The Main Node receives fuel information from the Fuel Node, monitors engine temperature using a DS18B20 sensor, and sends indicator commands to the Indicator Node.
# Block diagram
<img width="643" height="459" alt="image" src="https://github.com/user-attachments/assets/c1a458f1-651b-46d0-87d2-5731203adf7d" />

# Features

* CAN Bus Communication
* Distributed Multi-Node Architecture
* Engine Temperature Monitoring
* Fuel Level Monitoring
* Left and Right Indicator Control
* Real-Time LCD Display
* External Interrupt-Based Switching
* Automotive ECU Communication Simulation
  
# System Architecture
Fuel Node → CAN Bus → Main Node → CAN Bus → Indicator Node
# Main Node

* Reads engine temperature from DS18B20
* Receives fuel percentage via CAN
* Displays temperature and fuel level on LCD
* Detects left and right indicator switch presses
* Sends indicator commands through CAN
 
# Fuel Node

* Reads fuel sensor using ADC
* Converts ADC values into fuel percentage
* Transmits fuel data to Main Node through CAN

# Indicator Node

* Receives indicator commands from Main Node
* Controls LED-based turn indicators
* Left Indicator: LEDs scroll Right → Left
* Right Indicator: LEDs scroll Left → Right

# Hardware Components

* LPC2129 ARM7 Microcontroller
* MCP2551 CAN Transceiver
* DS18B20 Temperature Sensor
* 20x4 LCD Display
* Fuel Sensor / Potentiometer
* Push Buttons
* LED Array
* CAN Bus Network
  
# Indicator Commands

| CAN ID | Command | Description        |
| ------ | ------- | ------------------ |
| 0x11   | L       | Left Indicator ON  |
| 0x11   | R       | Right Indicator ON |
| 0x11   | O       | Indicator OFF      |

# Project Workflow

1. Fuel Node reads fuel sensor data using ADC.
2. Fuel percentage is transmitted over CAN.
3. Main Node receives and displays fuel data.
4. Main Node reads engine temperature from DS18B20.
5. User presses Left or Right indicator switch.
6. Main Node sends indicator command through CAN.
7. Indicator Node receives the command.
8. LEDs perform the corresponding indicator animation.
   
# Expected Output

<img width="1600" height="725" alt="WhatsApp Image 2026-06-19 at 16 24 45" src="https://github.com/user-attachments/assets/a25a870d-a1c5-4ea3-ab68-747536943ed4" />


# Indicator Node
Sequential LED scrolling representing vehicle turn indicators.
# Learning Outcomes

* CAN Protocol Implementation
* CAN Frame Transmission and Reception
* LPC2129 ARM7 Programming
* Embedded C Development
* ADC Interfacing
* DS18B20 Sensor Interfacing
* LCD Interfacing
* External Interrupt Handling
* Distributed Embedded Systems Design
 
# Technologies Used

* Embedded C
* LPC2129 ARM7
* CAN Protocol
* ADC
* DS18B20
* LCD Interface
* Keil µVision
* Flash Magic
