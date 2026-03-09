🏠 Smart Home Automation using ESP32
A Smart Home Automation System built using the ESP32.

The system enables automatic and remote control of home appliances using sensors, WiFi, and a mobile web dashboard.
This project demonstrates how IoT technology can improve home comfort, automation, and energy efficiency.

📌 Project Features

✨ Automatic lighting using motion detection
✨ Smart door system using servo motor
✨ Temperature monitoring using DHT11
✨ Mobile phone control through web browser
✨ LCD welcome message display
✨ WiFi based IoT system
✨ Low-cost smart home prototype

🧰 Hardware Components
Component	Description
ESP32	Main microcontroller with WiFi
DHT11	Temperature sensing
HC-SR04	Distance detection
SG90 Micro Servo Motor	Automatic door mechanism
16x2 LCD Display	Message and temperature display
IR Sensor	Motion detection
LEDs	Room lights (Bedroom, Hall, Kitchen)
Breadboard	Circuit connections
Jumper Wires	Wiring
💻 Software Requirements

Arduino IDE
ESP32 Board Package

Required Libraries:
WiFi
ESP32Servo
DHT Sensor Library
LiquidCrystal

🏗 System Architecture
        📱 Mobile Phone
             │
             │ WiFi
             ▼
        🌐 ESP32 Web Server
             │
   ┌─────────┼─────────┐
   │         │         │
 💡 LEDs    🚪 Servo   📡 Sensors
 Rooms      Door      IR + DHT11
                        │
                       📟 LCD

⚙️ Working Principle
1️⃣ System Initialization
ESP32 connects to WiFi network
LCD displays Smart Home Ready

2️⃣ Motion Detection
When a person enters:
IR sensor detects movement
Hall LED automatically turns ON
Servo motor opens the door

LCD displays:
WELCOME HOME

3️⃣ Temperature Monitoring
The DHT11 sensor measures room temperature.
Displayed on:
LCD screen
Web dashboard

4️⃣ Mobile Web Control
ESP32 hosts a web server.
Users open the ESP32 IP address in their phone browser to control devices.

Available controls:

✔ Bedroom Light ON/OFF
✔ Hall Light ON/OFF
✔ Kitchen Light ON/OFF
✔ Door OPEN/CLOSE
✔ View Temperature

📱 Web Dashboard
Example mobile interface:
SMART HOME
Temperature: 28°C

Bedroom Light
[ ON ]  [ OFF ]

Hall Light
[ ON ]  [ OFF ]

Kitchen Light
[ ON ]  [ OFF ]

Door Control
[ OPEN ] [ CLOSE ]

🎯 Objectives

Develop a smart home automation prototype
Implement IoT-based device control
Integrate sensors for automatic responses
Demonstrate real-time monitoring
Provide remote appliance control

🚀 Advantages

✔ Low-cost automation system
✔ Easy installation
✔ Energy efficient
✔ Remote device control
✔ Expandable IoT platform

🏢 Applications

Smart homes
Offices
Hotels
Energy management systems
Elderly assistance systems

🔮 Future Improvements
Future upgrades may include:
Mobile app development
Voice control with Google Assistant or Amazon Alexa
Cloud integration
Smart security camera
Multiple room automation

📷 Project Demonstration

Demo Flow:

1️⃣ Person enters → IR sensor detects motion
2️⃣ Hall light turns ON automatically
3️⃣ Door opens using servo motor
4️⃣ LCD displays WELCOME HOME
5️⃣ User controls devices through mobile dashboard

📚 Learning Outcomes
Through this project we learned:
IoT system design
ESP32 programming
Sensor integration
Web-based device control
Embedded system development

👨‍💻 Author
Anushka Yede
Vedant Yerne
Janhavi Deshmukh
Swayam Thakur

Smart Home Automation using ESP32
IoT Mini Project
