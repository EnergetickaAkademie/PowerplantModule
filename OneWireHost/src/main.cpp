#define PJON_INCLUDE_SWBB // Include SoftwareBitBang strategy
#include <Arduino.h>
#include <PJONSoftwareBitBang.h>
#include <ota.h>
#include "secrets.h"
#include <vector>

// PJON Configuration
#define PJON_PIN D1
#define MASTER_ID 1
#define HEARTBEAT_TIMEOUT_MS 3100  // 3.1 seconds timeout

// PJON instance using SoftwareBitBang strategy
PJON<SoftwareBitBang> bus(MASTER_ID);

// Slave tracking structure
struct SlaveInfo {
    uint8_t id;
    uint8_t type;
    unsigned long lastHeartbeat;
    
    // Constructor for easy initialization
    SlaveInfo(uint8_t _id, uint8_t _type) : id(_id), type(_type), lastHeartbeat(millis()) {}
};

// Dynamic slave management using vector
std::vector<SlaveInfo> slaves;

// Message types
enum MessageType {
    HEARTBEAT = 0x03
};

// Function declarations
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
void addOrUpdateSlave(uint8_t id, uint8_t type);
void checkSlaveTimeouts();
void printSlaveList();
std::vector<SlaveInfo>::iterator findSlave(uint8_t id);

void setup() {
  Serial.begin(115200);
  Serial.println("PJON Slave Discovery Master");
  
  // Initialize slave vector (empty by default)
  slaves.clear();
  slaves.reserve(20); // Reserve space for 20 slaves to avoid frequent reallocations
  
  // Connect to WiFi and setup OTA
  connectWifi(SECRET_SSID, SECRET_PASSWORD);
  setupOTA(-1, "PjonMaster");
  
  // Setup WebSerial debugging
  setupWebSerial("PjonMaster");
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize PJON
  bus.strategy.set_pin(PJON_PIN);
  bus.set_receiver(receiver_function);
  bus.set_acknowledge(false);          // Disable acknowledgments
  bus.set_crc_32(true);               // Enable 32-bit CRC for better error detection
  bus.set_packet_auto_deletion(true); // Auto-delete delivered packets
  bus.begin();
  
  Serial.print("PJON Master initialized with ID: ");
  Serial.println(MASTER_ID);
  Serial.print("Communication pin: ");
  Serial.println(PJON_PIN);
  Serial.println("ACK disabled, CRC-32 enabled");
  
  WebSerial.print("PJON Master ID: ");
  WebSerial.println(MASTER_ID);
  WebSerial.print("Pin: ");
  WebSerial.println(PJON_PIN);
  WebSerial.println("Slave discovery system ready - ACK disabled");
  
  Serial.println("Setup complete - waiting for slave heartbeats");
}

void loop() {
  // Handle OTA updates
  handleOTA();
  
  // Update PJON (handle incoming messages)
  bus.update();
  bus.receive();
  
  // Check for slave timeouts every 100ms
  static unsigned long lastTimeoutCheck = 0;
  if (millis() - lastTimeoutCheck > 100) {
    checkSlaveTimeouts();
    lastTimeoutCheck = millis();
  }
  
  // Print slave list every second
  static unsigned long lastListPrint = 0;
  if (millis() - lastListPrint > 1000) {
    printSlaveList();
    lastListPrint = millis();
  }
  
  // Small delay to prevent excessive CPU usage
}

// PJON message receiver function
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
    if (length < 1) return;
    
    uint8_t messageType = payload[0];
    
    /*Serial.print("Received message from slave ID ");
    Serial.print(packet_info.tx.id);
    Serial.print(", type: 0x");
    Serial.println(messageType, HEX);

    WebSerial.print("Received message from slave ID ");
    WebSerial.print(packet_info.tx.id);
    WebSerial.print(", type: 0x");
    WebSerial.println(messageType, HEX);
    WebSerial.flush();*/
    
    switch (messageType) {
        case HEARTBEAT:
            if (length >= 3) { // messageType + id + type
                uint8_t slaveId = payload[1];
                uint8_t slaveType = payload[2];
                
                // Use the actual sender ID from packet info, not payload
                addOrUpdateSlave(packet_info.tx.id, slaveType);
            }
            break;
            
        default:
            Serial.print("Unknown message type: 0x");
            Serial.println(messageType, HEX);
            break;
    }
}

// Slave management functions
std::vector<SlaveInfo>::iterator findSlave(uint8_t id) {
    for (auto it = slaves.begin(); it != slaves.end(); ++it) {
        if (it->id == id) {
            return it;
        }
    }
    return slaves.end(); // Not found
}

void addOrUpdateSlave(uint8_t id, uint8_t type) {
    auto it = findSlave(id);
    
    if (it != slaves.end()) {
        // Update existing slave
        it->lastHeartbeat = millis();
        it->type = type;
        // Don't print anything for updates - too verbose
    } else {
        // Add new slave
        slaves.emplace_back(id, type);
        
        Serial.print("New slave connected - ID ");
        Serial.print(id);
        Serial.print(", type ");
        Serial.println(type);
        
        WebSerial.print("New slave connected - ID ");
        WebSerial.print(id);
        WebSerial.print(", type ");
        WebSerial.println(type);
        WebSerial.flush();
    }
}

void checkSlaveTimeouts() {
    unsigned long currentTime = millis();
    
    // Use reverse iterator to safely remove elements while iterating
    for (auto it = slaves.begin(); it != slaves.end();) {
        if (currentTime - it->lastHeartbeat > HEARTBEAT_TIMEOUT_MS) {
            Serial.print("Slave ID ");
            Serial.print(it->id);
            Serial.println(" timed out - removing from list");
            
            WebSerial.print("Slave ID ");
            WebSerial.print(it->id);
            WebSerial.println(" disconnected (timeout)");
            WebSerial.flush();
            
            // Remove slave from vector
            it = slaves.erase(it);
        } else {
            ++it;
        }
    }
}

void printSlaveList() {
    WebSerial.print("Active slaves (");
    WebSerial.print(slaves.size());
    WebSerial.println("): ");
    
    if (slaves.empty()) {
        WebSerial.println("None");
    } else {
        for (const auto& slave : slaves) {
            WebSerial.printf("ID: %d, Type: %d\n", slave.id, slave.type);
        }
    }
    WebSerial.flush();
}
