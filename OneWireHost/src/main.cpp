#include <Arduino.h>
#include <com-prot.h>
#include <ota.h>
#include "secrets.h"

// Create master instance
ComProtMaster master(1, D1); // Master ID 1, pin D1

void setup() {
    Serial.begin(115200);
    Serial.println("PJON Slave Discovery Master");
    
    // Connect to WiFi and setup OTA
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    setupOTA(-1, "PjonMaster");
    setupWebSerial("PjonMaster");
    
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Initialize the master
    master.begin();
    
    Serial.println("PJON Master initialized with Com-Prot library");
    WebSerial.println("PJON Master ready - waiting for slave heartbeats");
}

void loop() {
    // Handle OTA updates
    handleOTA();
    
    // Update master (handles incoming messages and timeouts)
    master.update();
    
    // Example: Send commands to slaves every 10 seconds
    static unsigned long lastCommand = 0;
    if (millis() - lastCommand > 10000) {
        
        // Get all connected slaves
        auto allSlaves = master.getConnectedSlaves();
        Serial.printf("Connected slaves: %d\n", allSlaves.size());
        
        for (const auto& slave : allSlaves) {
            Serial.printf("Slave ID: %d, Type: %d\n", slave.id, slave.type);
        }
        
        // Example commands:
        
        // 1. Send LED toggle command (0x10) to all slaves of type 1
        if (master.getSlavesByType(1).size() > 0) {
            uint8_t ledState = (millis() / 10000) % 2; // Toggle every 10 seconds
            master.sendCommandToSlaveType(1, 0x10, &ledState, 1);
            Serial.printf("Sent LED command (%d) to type 1 slaves\n", ledState);
        }
        
        // 2. Send temperature request (0x20) to all slaves of type 2
        if (master.getSlavesByType(2).size() > 0) {
            master.sendCommandToSlaveType(2, 0x20);
            Serial.println("Sent temperature request to type 2 slaves");
        }
        
        lastCommand = millis();
    }
    
    // Print slave list every second
    static unsigned long lastListPrint = 0;
    if (millis() - lastListPrint > 1000) {
        auto slaves = master.getConnectedSlaves();
        WebSerial.printf("Active slaves (%d): ", slaves.size());
        
        if (slaves.empty()) {
            WebSerial.println("None");
        } else {
            for (const auto& slave : slaves) {
                WebSerial.printf("ID: %d, Type: %d ", slave.id, slave.type);
            }
            WebSerial.println();
        }
        WebSerial.flush();
        
        lastListPrint = millis();
    }
}
