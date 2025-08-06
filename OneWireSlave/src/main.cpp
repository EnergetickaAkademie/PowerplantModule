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
#include "PeripheralFactory.h"

// Configuration from build flags (set in platformio.ini)
#ifndef SLAVE_ID
#define SLAVE_ID 10  // Default fallback
#endif

#ifndef SLAVE_TYPE
#define SLAVE_TYPE 7 // Default fallback
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME "PjonSlave"  // Default fallback
#endif

// Create slave instance
ComProtSlave slave(SLAVE_ID, SLAVE_TYPE, D1); // Use build flags for ID and type

 bool otaMode = false; // Flag to indicate if OTA mode is enabled
PeripheralFactory factory; // Create a factory instance for peripherals
// LED pin for status indication
Atomizer* atomizer = nullptr; // Pointer to atomizer peripheral

void handleMistyCommand(uint8_t* data, uint16_t length, uint8_t senderId) {
    WebSerial.printf("Custom command from master %d, data length: %d\n", senderId, length);
    Serial.printf("Custom command from master %d, data length: %d\n", senderId, length);
    bool mistyCommand = false;
    if (length > 0) {
        mistyCommand = (data[0] == 1);
    }

    WebSerial.printf("Misty command received: %s current state: %s XORED result: %s\n", 
        mistyCommand ? "true" : "false", 
        atomizer->getTargetState() ? "Active" : "Inactive", 
        (mistyCommand != atomizer->getTargetState()) ? "true" : "false");
    
    if(mistyCommand != atomizer->getTargetState()) {
        atomizer->toggle();
        WebSerial.printf("Atomizer toggled, new state: %s\n", 
            atomizer->getTargetState() ? "Active" : "Inactive");
    }
}

void switchAtomizer() {
    if (atomizer) {
        atomizer->toggle();
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("PJON Temperature Sensor Slave with Com-Prot");
    Serial.flush();
    Serial.println("Booting");
    pinMode(D0, INPUT_PULLUP);        // HIGH by default
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
    slave.setCommandHandler(0x10, handleMistyCommand);      // Custom command
     
    // Initialize the slave
    slave.begin();
    
    Serial.printf("Com-Prot Slave initialized - ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    WebSerial.printf("Slave ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    WebSerial.println("Command handlers registered:");
    WebSerial.println("  0x30 - atomizer Command");
    WebSerial.println("Debug receive handler enabled");
    WebSerial.flush();
    
    Serial.println("Setup complete - sending heartbeats to master");
    atomizer = factory.createAtomizer(D2); // Create atomizer peripheral on pin D2
    //factory.createPeriodic(1000, switchAtomizer); // Create periodic task to switch atomizer every second
}

void loop()
{
    factory.update(); // Update all peripherals
    // Handle OTA updates
    if(otaMode)
    handleOTA();
    
    
    // Update slave (handles incoming messages and heartbeats)
    slave.update();
    
    
    
    // Status indication
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) { // Every 10 seconds
        WebSerial.println("Slave alive and listening otaMode: " + String(otaMode ? "Enabled" : "Disabled"));
        WebSerial.flush();
        lastStatus = millis();
    }



 
    // Small delay for stability
}