#include <Arduino.h>
#include <com-prot.h>
#include <ota.h>
#include "secrets.h"

// Create master instance
ComProtMaster master(1, D1); // Master ID 1, pin D1

// Debug receive handler - called for every received message
void debugReceiveHandler(uint8_t* payload, uint16_t length, uint8_t senderId, uint8_t messageType) {
    // Only log non-heartbeat messages to avoid spam
    if (messageType != 0x03) { // Skip heartbeat messages
        Serial.printf("[DEBUG] RX from slave %d: type=0x%02X, len=%d\n", senderId, messageType, length);
        WebSerial.printf("[DEBUG] RX: ID=%d, Type=0x%02X, Len=%d\n", senderId, messageType, length);
        WebSerial.flush();
    }
    
    // Log heartbeat messages with less detail
    if (messageType == 0x03) {
        static unsigned long lastHeartbeatLog = 0;
        if (millis() - lastHeartbeatLog > 5000) { // Log every 5 seconds
            Serial.printf("[DEBUG] Heartbeats active from %d slaves\n", master.getSlaveCount());
            lastHeartbeatLog = millis();
        }
    }
}

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
    
    // Set debug receive handler
    master.setDebugReceiveHandler(debugReceiveHandler);
    
    // Initialize the master
    master.begin();
    
    Serial.println("PJON Master initialized with Com-Prot library and debug handler");
    WebSerial.println("PJON Master ready - debug handler enabled");
    WebSerial.flush();
}

void loop() {
    // Handle OTA updates
    handleOTA();
    
    // Update master (handles incoming messages and timeouts)
    master.update();
    
    // Example: Send commands to slaves every 10 seconds
    static unsigned long lastCommand = 0;
    if (millis() - lastCommand > 5000) {
        
        // Get all connected slaves
        auto allSlaves = master.getConnectedSlaves();
        WebSerial.printf("Connected slaves: %d\n", allSlaves.size());
        
        for (const auto& slave : allSlaves) {
            WebSerial.printf("Slave ID: %d, Type: %d\n", slave.id, slave.type);
        }
        
        // Example commands:
        
        // 1. Send LED toggle command (0x10) to all slaves of type 1 using broadcast
        if (master.getSlavesByType(1).size() > 0) {
            uint8_t ledState = (millis() / 5000) % 2; // Toggle every 5 seconds
            master.sendCommandToSlaveType(1, 0x10, &ledState, 1);
            WebSerial.printf("Sent LED broadcast command (%d) to type 1 slaves\n", ledState);
        }
        
        // 2. Send temperature request (0x20) to all slaves of type 2 using broadcast
        if (master.getSlavesByType(2).size() > 0) {
            master.sendCommandToSlaveType(2, 0x20);

            WebSerial.println("Sent temperature request broadcast to type 2 slaves");
        }
        
        lastCommand = millis();
    }
    /*
    // Print slave list every second
    static unsigned long lastListPrint = 0;
    if (millis() - lastListPrint > 1000) {
        auto slaves = master.getConnectedSlaves();
        //WebSerial.printf("Active slaves (%d): ", slaves.size());
        
        if (slaves.empty()) {
            //WebSerial.println("None");
        } else {
            for (const auto& slave : slaves) {
                WebSerial.printf("ID: %d, Type: %d ", slave.id, slave.type);
            }
            WebSerial.println();
        }
        WebSerial.flush();
        
        lastListPrint = millis();
    }*/
}
