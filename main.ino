#include <WiFi.h>              // Enables WiFi functionality
#include <WebServer.h>         // HTTP server library
#include <WebSocketsServer.h>  // WebSocket for real-time communication
#include <ESP32Servo.h>        // Servo motor control library
#include "model.h"             // Includes crop prediction logic

// ---------------- PIN DEFINITIONS ----------------
#define REDE 15       // RS485 direction control pin
#define AIN1 14       // Motor A direction pin 1
#define AIN2 12       // Motor A direction pin 2
#define PWMA 13       // Motor A speed control pin
#define BIN1 25       // Motor B direction pin 1
#define BIN2 26       // Motor B direction pin 2
#define PWMB 27       // Motor B speed control pin
#define STBY 33       // Motor driver standby pin
#define SERVO_PIN 22  // Servo motor pin

// Ultrasonic sensor pins (Front, Left, Right)
#define TRIG_F 2
#define ECHO_F 4
#define TRIG_L 5
#define ECHO_L 18
#define TRIG_R 19
#define ECHO_R 23

// ---------------- NPK SENSOR COMMANDS ----------------
const byte nitro_c[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C}; // Nitrogen command
const byte phos_c[]  = {0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC}; // Phosphorus command
const byte pota_c[]  = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0}; // Potassium command

// ---------------- OBJECTS ----------------
WebServer server(80);              // HTTP server on port 80
WebSocketsServer webSocket(81);    // WebSocket server on port 81
Servo seeder;                     // Servo object

// ---------------- STATE VARIABLES ----------------
bool autoMode = false;     // Autonomous mode flag
bool seedEnabled = false;  // Seed dispensing flag
bool isFwd = false;        // Tracks forward movement
bool isLocked = false;     // Locks system during analysis

int n_v, p_v, k_v;         // Stores NPK values
int botSpeed = 210;        // Default speed

unsigned long lastNPK = 0;     // Timer for NPK reading
unsigned long lastSeed = 0;    // Timer for seed dispensing
unsigned long lockStart = 0;   // Timer for soil analysis

// ---------------- MOTOR CONTROL ----------------
void drive(int sL, int sR) {

  // Left motor control
  digitalWrite(AIN1, sL > 0 ? HIGH : LOW);  // Forward or stop
  digitalWrite(AIN2, sL < 0 ? HIGH : LOW);  // Reverse or stop
  analogWrite(PWMA, abs(sL));               // Speed control

  // Right motor control
  digitalWrite(BIN1, sR > 0 ? HIGH : LOW);
  digitalWrite(BIN2, sR < 0 ? HIGH : LOW);
  analogWrite(PWMB, abs(sR));

  // Check if moving forward
  isFwd = (sL > 100 && sR > 100);
}

// ---------------- READ NPK SENSOR ----------------
int readNPK(const byte* cmd) {

  while (Serial2.available()) Serial2.read();  // Clear buffer

  digitalWrite(REDE, HIGH);  // Enable transmit mode
  delay(10);

  Serial2.write(cmd, 8);     // Send command
  Serial2.flush();

  digitalWrite(REDE, LOW);   // Switch to receive mode

  unsigned long start = millis();

  // Wait for response (timeout)
  while (Serial2.available() < 7 && millis() - start < 300) {
    yield();
  }

  if (Serial2.available() >= 7) {
    byte r[7];

    for (int i = 0; i < 7; i++) {
      r[i] = Serial2.read();  // Read response
    }

    return (int)((r[3] << 8) | r[4]); // Convert to value
  }

  return 0; // Error case
}

// ---------------- ULTRASONIC SENSOR ----------------
long ping(int trig, int echo) {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);

  digitalWrite(trig, LOW);

  long distance = pulseIn(echo, HIGH, 20000) / 58; // Convert to cm

  return (distance == 0) ? 400 : distance; // Default if no object
}

// ---------------- WEB INTERFACE ----------------
const char* html = "<h1>AgroSmart Pro Running</h1>"; // Simple UI

// ---------------- WEBSOCKET HANDLER ----------------
void onWs(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

  if (type == WStype_TEXT) {

    String cmd = String((char*)payload); // Convert to string

    if (cmd.startsWith("v")) {           // Speed command
      botSpeed = cmd.substring(1).toInt();
      return;
    }

    if (isLocked) return; // Ignore if locked

    if (cmd == "a") autoMode = !autoMode;       // Toggle auto
    else if (cmd == "f") drive(botSpeed, botSpeed); // Forward
    else if (cmd == "b") drive(-botSpeed, -botSpeed); // Backward
    else if (cmd == "l") drive(botSpeed, -botSpeed);  // Left
    else if (cmd == "r") drive(-botSpeed, botSpeed);  // Right
    else if (cmd == "s") drive(0, 0);                // Stop
    else if (cmd == "son") seedEnabled = true;       // Enable seed
    else if (cmd == "soff") seedEnabled = false;     // Disable seed

    else if (cmd == "p") {  // Start soil analysis
      drive(0, 0);
      isLocked = true;
      lockStart = millis();
    }
  }
}

// ---------------- SETUP ----------------
void setup() {

  Serial.begin(115200);  // Debug
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Sensor

  pinMode(REDE, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // Enable motor driver

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(TRIG_F, OUTPUT);
  pinMode(ECHO_F, INPUT);

  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);

  seeder.attach(SERVO_PIN); // Attach servo
  seeder.write(0);          // Initial position

  WiFi.softAP("AgroSmart_Pro", "agribot123"); // Create hotspot

  server.on("/", []() {
    server.send(200, "text/html", html); // Serve UI
  });

  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWs);
}

// ---------------- LOOP ----------------
void loop() {

  server.handleClient(); // Handle HTTP
  webSocket.loop();      // Handle WebSocket

  // AUTO NAVIGATION
  if (autoMode) {
    long f = ping(TRIG_F, ECHO_F);
    long l = ping(TRIG_L, ECHO_L);
    long r = ping(TRIG_R, ECHO_R);

    if (f < 30) {
      drive(0, 0);
      if (l > r) drive(botSpeed, -botSpeed);
      else drive(-botSpeed, botSpeed);
    } else {
      drive(botSpeed, botSpeed);
    }
  }

  // NPK READING
  if (!isLocked && millis() - lastNPK > 2500) {
    n_v = readNPK(nitro_c);
    p_v = readNPK(phos_c);
    k_v = readNPK(pota_c);
    lastNPK = millis();
  }

  // CROP PREDICTION
  if (isLocked && millis() - lockStart > 4000) {
    String result = predictCrop(n_v, p_v, k_v, false);
    isLocked = false;
  }

  // SEED DISPENSING
  if (seedEnabled && isFwd && millis() - lastSeed > 3000) {
    seeder.write(60);
    delay(150);
    seeder.write(0);
    lastSeed = millis();
  }
}
