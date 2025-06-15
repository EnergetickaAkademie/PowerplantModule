#define PJON_INCLUDE_SWBB // Include SoftwareBitBang strategy
#include <Arduino.h>
#include <PJONSoftwareBitBang.h>
#include <ota.h>
#include "secrets.h"

// PJON Configuration
#define PJON_PIN D1
#define MASTER_ID 1

// PJON instance using SoftwareBitBang strategy
PJON<SoftwareBitBang> bus(MASTER_ID);

// Slave device IDs to poll
uint8_t slave_ids[] = {10}; // Add more slave IDs as needed
uint8_t num_slaves = sizeof(slave_ids) / sizeof(slave_ids[0]);
uint8_t current_slave_index = 0;

// Message types
enum MessageType {
    HELLO_REQUEST = 0x01,
    HELLO_RESPONSE = 0x02,
    HEARTBEAT = 0x03
};

// Function declarations
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
void sendHelloRequest();
void sendHeartbeat();

void setup() {
  Serial.begin(115200);
  Serial.println("PJON Hello World Master");
  
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
  bus.begin();
  
  Serial.print("PJON Master initialized with ID: ");
  Serial.println(MASTER_ID);
  Serial.print("Communication pin: ");
  Serial.println(PJON_PIN);
  
  WebSerial.print("PJON Master ID: ");
  WebSerial.println(MASTER_ID);
  WebSerial.print("Pin: ");
  WebSerial.println(PJON_PIN);
  
  Serial.println("Setup complete");
}

void loop() {
  // Handle OTA updates
  handleOTA();
  
  // Update PJON (handle incoming messages)
  bus.update();
  bus.receive();
  
  // Send hello request every 5 seconds
  static unsigned long lastHello = 0;
  if (millis() - lastHello > 5000) {
    sendHelloRequest();
    lastHello = millis();
  }
  
  // Send heartbeat every 10 seconds
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat > 10000) {
    sendHeartbeat();
    lastHeartbeat = millis();
  }
  
  // Small delay to prevent excessive CPU usage
  delay(1);
}

// PJON message receiver function
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
    if (length < 1) return;
    
    uint8_t messageType = payload[0];
    
    Serial.print("Received message from slave ID ");
    Serial.print(packet_info.tx.id);
    Serial.print(", type: 0x");
    Serial.println(messageType, HEX);
    
    switch (messageType) {
        case HELLO_RESPONSE:
            if (length > 1) {
                // Create properly sized buffer with null termination
                size_t msgLen = length - 1;  // Subtract 1 for message type byte
                char message[msgLen + 1];    // +1 for null terminator
                
                // Copy message and ensure null termination
                memcpy(message, &payload[1], msgLen);
                message[msgLen] = '\0';      // Explicit null termination
                
                Serial.print("Hello response from slave ");
                Serial.print(packet_info.tx.id);
                Serial.print(": ");
                Serial.println(message);
                
                WebSerial.print("Slave ");
                WebSerial.print(packet_info.tx.id);
                WebSerial.print(": ");
                WebSerial.println(message);
                WebSerial.flush();
            }
            break;
            
        default:
            Serial.print("Unknown message type: 0x");
            Serial.println(messageType, HEX);
            break;
    }
}

void sendHelloRequest()
{
    if (num_slaves == 0) return;
    
    uint8_t slave_id = slave_ids[current_slave_index];
    
    // Send hello request
    uint8_t request[1] = {HELLO_REQUEST};
    uint8_t result = bus.send(slave_id, request, sizeof(request));
    
    if (result == PJON_ACK) {
        Serial.print("Hello request sent to slave ");
        Serial.println(slave_id);
        WebSerial.print("Hello request sent to slave ");
        WebSerial.println(slave_id);
        WebSerial.flush();
    } else {
        Serial.print("Failed to send hello request to slave ");
        Serial.println(slave_id);
        WebSerial.print("Failed to reach slave ");
        WebSerial.println(slave_id);
        WebSerial.flush();
    }
    
    // Move to next slave
    current_slave_index = (current_slave_index + 1) % num_slaves;
}

void sendHeartbeat()
{
    uint8_t heartbeat[1] = {HEARTBEAT};
    
    // Send heartbeat to all slaves
    for (int i = 0; i < num_slaves; i++) {
        uint8_t result = bus.send(slave_ids[i], heartbeat, sizeof(heartbeat));
        
        if (result == PJON_ACK) {
            Serial.print("Heartbeat sent to slave ");
            Serial.println(slave_ids[i]);
        } else {
            Serial.print("Heartbeat failed for slave ");
            Serial.println(slave_ids[i]);
        }
        delay(10); // Small delay between heartbeats to prevent bus flooding
    }
}
