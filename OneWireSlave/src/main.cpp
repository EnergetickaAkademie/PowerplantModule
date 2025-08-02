/*
 *    PJON Temperature Sensor Slave
 *    Using Com-Prot Library for simplified communication
 *    More stable and reliable than OneWire communication
 *
 *    Com-Prot Features:
 *    - Automatic heartbeat management
 *    - Command handler registration
 *    - Robust error handling and acknowledgments
 *    - Multi-master support
 *    - Better timing tolerance
 *    - Collision detection and retry logic
 */

#include <Arduino.h>
#include <com-prot.h>
#include <ota.h>
#include "secrets.h"

// Configuration from build flags (set in platformio.ini)
#ifndef SLAVE_ID
#define SLAVE_ID 10  // Default fallback
#endif

#ifndef SLAVE_TYPE
#define SLAVE_TYPE 1 // Default fallback
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME "PjonSlave"  // Default fallback
#endif

// Create slave instance
ComProtSlave slave(SLAVE_ID, SLAVE_TYPE, D1); // Use build flags for ID and type

// LED control
constexpr uint8_t pin_led{LED_BUILTIN};
bool ledBlinking = false;
unsigned long blinkStartTime = 0;
 bool otaMode = false; // Flag to indicate if OTA mode is enabled
// Command handlers
void handleLedCommand(uint8_t* data, uint16_t length, uint8_t senderId) {
    if (length > 0) {
        bool ledState = data[0];
        digitalWrite(pin_led, ledState ? LOW : HIGH); // LED is inverted on ESP8266
        
        Serial.printf("LED turned %s by master %d\n", ledState ? "ON" : "OFF", senderId);
        WebSerial.printf("LED command received: %s\n", ledState ? "ON" : "OFF");
        WebSerial.flush();
    }
}


void handleMistyCommand(uint8_t* data, uint16_t length, uint8_t senderId) {
    WebSerial.printf("Custom command from master %d, data length: %d\n", senderId, length);
    
    bool mistyCommand = false;
    if (length > 0) {
        mistyCommand = (data[0] == 1); // Example condition, adjust as needed
    }
    digitalWrite(pin_led, mistyCommand ? LOW : HIGH); // LED is inverted on ESP8266
    
}

// Debug receive handler - called for every received message
void debugReceiveHandler(uint8_t* payload, uint16_t length, uint8_t senderId, uint8_t messageType) {
    Serial.printf("[DEBUG] RX from master %d: type=0x%02X, len=%d\n", senderId, messageType, length);
    
    // Log to WebSerial with less spam
    static unsigned long lastDebugLog = 0;
    WebSerial.printf("[DEBUG] RX: ID=%d, Type=0x%02X, Len=%d\n", senderId, messageType, length);
    WebSerial.flush();
    lastDebugLog = millis();
    
}

void blinkLed() {
    static unsigned long lastBlink = 0;
    static bool blinkState = false;
    
    if (ledBlinking) {
        if (millis() - lastBlink > 200) {
            blinkState = !blinkState;
            digitalWrite(pin_led, blinkState ? LOW : HIGH); // LED is inverted
            lastBlink = millis();
        }
        
        // Stop blinking after 5 seconds
        if (millis() - blinkStartTime > 5000) {
            ledBlinking = false;
            digitalWrite(pin_led, HIGH); // Turn off LED
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("PJON Temperature Sensor Slave with Com-Prot");
    Serial.flush();
    Serial.println("Booting");
    
    pinMode(pin_led, OUTPUT);
    digitalWrite(pin_led, HIGH); // Turn off LED initially (inverted)
    
    // Configure D0 for OTA mode detection
    pinMode(D0, INPUT_PULLUP);        // HIGH by default
    pinMode(D2, OUTPUT); 
    digitalWrite(D2, HIGH); // Ensure D2 is set as output for Com-Prot communication
    
    // Check if D0 is low to enable OTA mode
    otaMode = !digitalRead(D0);  // LOW = OTA mode enabled
    
    // Connect to WiFi and setup OTA
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    setupOTA(-1, DEVICE_NAME);
    setupWebSerial(DEVICE_NAME);
    
    if (otaMode) {
        Serial.println("OTA Mode enabled - D0 is LOW");
        WebSerial.println("OTA Mode enabled - D0 is LOW");
        WebSerial.flush();
        
        // In OTA mode, we'll wait for updates and not proceed with normal operation
        while (true) {
            handleOTA();
            delay(100);
        }
    } else {
        Serial.println("Normal mode - D0 is HIGH");
        WebSerial.println("Normal mode - D0 is HIGH");
        WebSerial.flush();
    }

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
      // Register command handlers
    slave.setCommandHandler(0x10, handleLedCommand);        // LED control
    slave.setCommandHandler(0x30, handleMistyCommand);      // Custom command
    
    // Set debug receive handler
    slave.setDebugReceiveHandler(debugReceiveHandler);
    
    // Initialize the slave
    slave.begin();
    
    Serial.printf("Com-Prot Slave initialized - ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
      WebSerial.printf("Slave ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    WebSerial.println("Command handlers registered:");
    WebSerial.println("  0x10 - LED Control");
    WebSerial.println("  0x20 - Temperature Request");
    WebSerial.println("  0x30 - Custom Command");
    WebSerial.println("Debug receive handler enabled");
    WebSerial.flush();
    
    Serial.println("Setup complete - sending heartbeats to master");
}

void loop()
{
    // Handle OTA updates
    if(otaMode)
    handleOTA();
    
    
    // Update slave (handles incoming messages and heartbeats)
    slave.update();
    
    // Handle LED blinking if active
    blinkLed();
    
    // Status indication
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) { // Every 10 seconds
        WebSerial.println("Slave alive and listening otaMode: " + String(otaMode ? "Enabled" : "Disabled"));
        WebSerial.flush();
        lastStatus = millis();
    }
    
    // Small delay for stability
}