/*
 * Direct PJON Master Test
 * Tests broadcast functionality without com-prot library
 * 
 * This will help debug issues with broadcast commands
 */

#include <Arduino.h>
#include <ota.h>
#include "secrets.h"

#define PJON_INCLUDE_SWBB
#include <PJONSoftwareBitBang.h>

// PJON instance
PJON<SoftwareBitBang> bus(1); // Master ID = 1

// Message types (same as com-prot)
#define MSG_HEARTBEAT 0x03
#define MSG_COMMAND   0x04

// Command types
#define CMD_LED_CONTROL    0x10
#define CMD_TEMP_REQUEST   0x20
#define CMD_CUSTOM         0x30

// Slave tracking
struct SlaveInfo {
    uint8_t id;
    uint8_t type;
    unsigned long lastSeen;
};

std::vector<SlaveInfo> slaves;
unsigned long lastSlaveCheck = 0;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    Serial.printf("[RX] From %d: ", packet_info.tx.id);
    for (uint16_t i = 0; i < length; i++) {
        Serial.printf("0x%02X ", payload[i]);
    }
    Serial.printf("(len=%d)\n", length);
    
    WebSerial.printf("[RX] From %d: ", packet_info.tx.id);
    for (uint16_t i = 0; i < length; i++) {
        WebSerial.printf("0x%02X ", payload[i]);
    }
    WebSerial.printf("(len=%d)\n", length);
    WebSerial.flush();
    
    if (length >= 3 && payload[0] == MSG_HEARTBEAT) {
        uint8_t slaveId = payload[1];
        uint8_t slaveType = payload[2];
        
        // Update or add slave
        bool found = false;
        for (auto& slave : slaves) {
            if (slave.id == slaveId) {
                slave.lastSeen = millis();
                slave.type = slaveType;
                found = true;
                break;
            }
        }
        
        if (!found) {
            slaves.push_back({slaveId, slaveType, millis()});
            Serial.printf("New slave discovered: ID=%d, Type=%d\n", slaveId, slaveType);
            WebSerial.printf("New slave: ID=%d, Type=%d\n", slaveId, slaveType);
            WebSerial.flush();
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Direct PJON Master Test");
    
    // Connect to WiFi and setup OTA
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    setupOTA(-1, "PjonMasterTest");
    setupWebSerial("PjonMasterTest");
    
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setup PJON
    bus.strategy.set_pin(D1);
    bus.set_receiver(receiver_function);
    bus.set_acknowledge(false);
    bus.set_crc_32(true);
    bus.set_packet_auto_deletion(true);
    bus.begin();
    
    Serial.println("PJON Master initialized");
    WebSerial.println("PJON Master ready - waiting for slaves");
    WebSerial.flush();
}

void sendBroadcastCommand(uint8_t slaveType, uint8_t command, uint8_t* data = nullptr, uint8_t dataLen = 0) {
    uint8_t messageLen = 3 + dataLen; // MSG_COMMAND + slaveType + command + data
    uint8_t* message = new uint8_t[messageLen];
    
    message[0] = MSG_COMMAND;
    message[1] = slaveType;  // Target slave type
    message[2] = command;
    
    if (data && dataLen > 0) {
        memcpy(&message[3], data, dataLen);
    }
    
    Serial.printf("[TX] Broadcasting to type %d: ", slaveType);
    for (uint8_t i = 0; i < messageLen; i++) {
        Serial.printf("0x%02X ", message[i]);
    }
    Serial.println();
    
    WebSerial.printf("[TX] Broadcast to type %d: ", slaveType);
    for (uint8_t i = 0; i < messageLen; i++) {
        WebSerial.printf("0x%02X ", message[i]);
    }
    WebSerial.println();
    WebSerial.flush();
    
    // Send broadcast
    uint16_t result = bus.send_packet(10, message, messageLen);
    
    Serial.printf("Broadcast result: %d %s\n", result, (result == PJON_ACK) ? "SUCCESS" : "FAILED");
    WebSerial.printf("Result: %s\n", (result == PJON_ACK) ? "SUCCESS" : "FAILED");
    WebSerial.flush();
    
    delete[] message;
}

void sendUnicastCommand(uint8_t slaveId, uint8_t command, uint8_t* data = nullptr, uint8_t dataLen = 0) {
    uint8_t messageLen = 3 + dataLen; // MSG_COMMAND + 0 + command + data
    uint8_t* message = new uint8_t[messageLen];
    
    message[0] = MSG_COMMAND;
    message[1] = 0;  // 0 = unicast
    message[2] = command;
    
    if (data && dataLen > 0) {
        memcpy(&message[3], data, dataLen);
    }
    
    Serial.printf("[TX] Unicast to slave %d: ", slaveId);
    for (uint8_t i = 0; i < messageLen; i++) {
        Serial.printf("0x%02X ", message[i]);
    }
    Serial.println();
    
    WebSerial.printf("[TX] Unicast to slave %d: ", slaveId);
    for (uint8_t i = 0; i < messageLen; i++) {
        WebSerial.printf("0x%02X ", message[i]);
    }
    WebSerial.println();
    WebSerial.flush();
    
    // Send unicast
    uint16_t result = bus.send(slaveId, message, messageLen);
    
    Serial.printf("Unicast result: %d %s\n", result, (result == PJON_ACK) ? "SUCCESS" : "FAILED");
    WebSerial.printf("Result: %s\n", (result == PJON_ACK) ? "SUCCESS" : "FAILED");
    WebSerial.flush();
    
    delete[] message;
}

void removeTimedOutSlaves() {
    unsigned long currentTime = millis();
    slaves.erase(
        std::remove_if(slaves.begin(), slaves.end(),
            [currentTime](const SlaveInfo& slave) {
                return (currentTime - slave.lastSeen) > 5000; // 5 second timeout
            }),
        slaves.end()
    );
}

void loop() {
    handleOTA();
    bus.update();
    bus.receive();
    
    // Remove timed out slaves every second
    if (millis() - lastSlaveCheck > 1000) {
        removeTimedOutSlaves();
        lastSlaveCheck = millis();
    }
    
    // Send test commands every 10 seconds
    static unsigned long lastCommand = 0;
    if (millis() - lastCommand > 9300) {
        
        Serial.printf("Active slaves: %d\n", slaves.size());
        WebSerial.printf("Active slaves: %d\n", slaves.size());
        
        for (const auto& slave : slaves) {
            Serial.printf("  Slave ID=%d, Type=%d\n", slave.id, slave.type);
            WebSerial.printf("  ID=%d, Type=%d\n", slave.id, slave.type);
        }
        WebSerial.flush();
        
        // Test 1: LED toggle broadcast to type 1 slaves
        bool hasType1 = false;
        for (const auto& slave : slaves) {
            if (slave.type == 1) {
                hasType1 = true;
                break;
            }
        }
        
        if (hasType1) {
            uint8_t ledState = (millis() / 10000) % 2;
            Serial.printf("Sending LED broadcast (state=%d) to type 1 slaves\n", ledState);
            WebSerial.printf("LED broadcast: %s\n", ledState ? "ON" : "OFF");
            sendBroadcastCommand(1, CMD_LED_CONTROL, &ledState, 1);
        }
        
        // Test 2: Temperature request broadcast to type 2 slaves
        bool hasType2 = false;
        for (const auto& slave : slaves) {
            if (slave.type == 2) {
                hasType2 = true;
                break;
            }
        }
        
        if (hasType2) {
            Serial.println("Sending temperature request broadcast to type 2 slaves");
            WebSerial.println("Temperature request broadcast");
            sendBroadcastCommand(2, CMD_TEMP_REQUEST);
        }
        
        // Test 3: Custom command unicast to specific slave
        bool hasSlaveId10 = false;
        for (const auto& slave : slaves) {
            if (slave.id == 10) {
                hasSlaveId10 = true;
                break;
            }
        }
        
        if (hasSlaveId10) {
            uint8_t customData[] = {0xAA, 0xBB, 0xCC, 0xDD};
            Serial.println("Sending custom unicast command to slave 10");
            WebSerial.println("Custom unicast to slave 10");
            sendUnicastCommand(10, CMD_CUSTOM, customData, sizeof(customData));
        }
        
        lastCommand = millis();
    }
    
}
