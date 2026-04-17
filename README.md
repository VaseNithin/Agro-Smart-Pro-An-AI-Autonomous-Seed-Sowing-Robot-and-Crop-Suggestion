Agro Smart Pro – IoT-Based Smart Agriculture System

Agro Smart Pro is an intelligent IoT-based agriculture robot built using ESP32 that performs real-time soil analysis, autonomous navigation, and smart crop recommendation. The system integrates embedded hardware with a web-based control dashboard for efficient farm automation.

*🌟 Features
* 🌱 Soil Nutrient Analysis
- Reads real-time Nitrogen (N), Phosphorus (P), Potassium (K) values
- Uses serial communication (Modbus-like protocol)
- Displays live data on web dashboard
* 🤖 Autonomous Navigation
- Obstacle detection using ultrasonic sensors
- Smart movement decisions (forward, turn, avoid obstacles)
- Auto and manual mode switching
*🌐 Web-Based Control Dashboard
- Hosted on ESP32 (no external server required)
- Built using HTML, CSS, JavaScript
- Real-time communication using WebSockets
- Mobile-friendly UI with touch controls
* 🌾 Crop Recommendation System
- Rule-based prediction algorithm
- Suggests suitable crops based on soil nutrients
- Instant results displayed in UI
* 🌿 Seed Dispensing Mechanism
- Servo-based automated seed dispenser
- Activates during forward movement
- Controlled via dashboard

* 🛠️ Tech Stack
* Hardware:
- ESP32 Microcontroller
- NPK Soil Sensor
- Ultrasonic Sensors (Front, Left, Right)
- Motor Driver (L298N or similar)
- Servo Motor
* Software:
- Embedded C++ (Arduino Framework)
- Web Server (HTTP)
- WebSockets (Real-time communication)
- HTML, CSS, JavaScript (Frontend UI)
        
* ⚙️ System Architecture
- ESP32 collects soil nutrient data via serial communication
- Sensor data is processed and sent to the web dashboard using WebSockets
- User can control the robot manually or enable autonomous mode
- Crop prediction logic analyzes NPK values and suggests crops
- Seed dispensing mechanism operates based on movement
* <img width="2819" height="1156" alt="mermaid-diagram" src="https://github.com/user-attachments/assets/e63bf84b-fce7-47c0-9374-8db9d29a89e8" />
* 🧠 Crop Prediction Logic
* The system uses a rule-based decision algorithm to recommend crops based on NPK values:
- High Nitrogen → Rice, Maize, Cotton
- High Phosphorus & Potassium → Fruits like Grapes, Papaya
- Low Nitrogen → Pulses like Lentils
- Balanced Soil → Coconut, Arecanut
        
* 🚀 How to Run
- Upload the code to ESP32 using Arduino IDE
- Connect all sensors and components properly
- Power the system
- Connect to WiFi:
- SSID: AgroSmart_Pro
- Password: agribot123
* Open browser and go to:
- http://192.168.4.1
- Control and monitor using the dashboard

* 📊 Key Highlights
- Real-time IoT system with live data streaming
- Full-stack integration (Embedded + Web UI)
- Autonomous robotics + smart agriculture
- Lightweight AI-like decision system (rule-based)
* 🔮 Future Enhancements
- Integration with cloud platforms (AWS / Firebase)
- Machine Learning-based crop prediction
- Mobile app development
- GPS-based navigation
- Weather data integration
      
