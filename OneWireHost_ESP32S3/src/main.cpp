#include <Arduino.h>
#include <com-prot.h>

// Create master instance
// Using GPIO 19 for OneWire communication on ESP32-S3
ComProtMaster master(1, 19); // Master ID 1, pin GPIO 19

// Debug receive handler - called for every received message
void debugReceiveHandler(uint8_t* payload, uint16_t length, uint8_t senderId, uint8_t messageType) {
    // Only log non-heartbeat messages to avoid spam
    if (messageType != 0x03) { // Skip heartbeat messages
        Serial.printf("[DEBUG] RX from slave %d: type=0x%02X, len=%d\n", senderId, messageType, length);
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
    
    // Wait for USB Serial to be ready (important for ESP32-S3)
    while (!Serial && millis() < 5000) {
        delay(10);
    }
    
    Serial.println("PJON Slave Discovery Master - ESP32-S3");
    Serial.println("Board: ESP32-S3-DEV-KIT-N8R8");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM size: %d bytes\n", ESP.getPsramSize());
    
    Serial.println("Ready");
    
    // Set debug receive handler
    master.setDebugReceiveHandler(debugReceiveHandler);
    
    // Initialize the master
    master.begin();
    
    Serial.println("PJON Master initialized with Com-Prot library and debug handler");
    Serial.printf("OneWire pin: GPIO %d\n", 19);
}

void loop() {
    // Update master (handles incoming messages and timeouts)
    master.update();
    
    // Example: Send commands to slaves every 5 seconds
    static unsigned long lastCommand = 0;
    if (millis() - lastCommand > 5000) {
        
        // Get all connected slaves
        auto allSlaves = master.getConnectedSlaves();
        Serial.printf("Connected slaves: %d\n", allSlaves.size());
        
        for (const auto& slave : allSlaves) {
            Serial.printf("Slave ID: %d, Type: %d\n", slave.id, slave.type);
        }
        
        // Example commands:
        
        // 1. Send LED toggle command (0x10) to all slaves of type 1 using broadcast
        if (master.getSlavesByType(1).size() > 0) {
            uint8_t ledState = (millis() / 5000) % 2; // Toggle every 5 seconds
            master.sendCommandToSlaveType(1, 0x10, &ledState, 1);
            Serial.printf("Sent LED broadcast command (%d) to type 1 slaves\n", ledState);
        }
        
        // 2. Send temperature request (0x20) to all slaves of type 2 using broadcast
        if (master.getSlavesByType(2).size() > 0) {
            master.sendCommandToSlaveType(2, 0x20);
            Serial.println("Sent temperature request broadcast to type 2 slaves");
        }
        
        // 3. Memory status update
        static unsigned long lastMemoryLog = 0;
        if (millis() - lastMemoryLog > 30000) { // Every 30 seconds
            Serial.printf("Free heap: %d bytes, Min free heap: %d bytes\n", 
                         ESP.getFreeHeap(), ESP.getMinFreeHeap());
            lastMemoryLog = millis();
        }
        
        lastCommand = millis();
    }
    
    // Optional: Uncomment to print slave list every second
    /*
    static unsigned long lastListPrint = 0;
    if (millis() - lastListPrint > 1000) {
        auto slaves = master.getConnectedSlaves();
        Serial.printf("Active slaves (%d): ", slaves.size());
        
        if (slaves.empty()) {
            Serial.println("None");
        } else {
            for (const auto& slave : slaves) {
                Serial.printf("ID: %d, Type: %d ", slave.id, slave.type);
            }
            Serial.println();
        }
        
        lastListPrint = millis();
    }
    */
}
