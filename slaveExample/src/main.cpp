/*
 * Direct PJON Slave Test
 * Tests broadcast reception without com-prot library
 * 
 * This will help debug issues with broadcast commands
 */

#include <Arduino.h>
#include <ota.h>
#include "secrets.h"

#define PJON_INCLUDE_SWBB
#include <PJONSoftwareBitBang.h>

// Configuration from build flags
#ifndef SLAVE_ID
#define SLAVE_ID 10
#endif

#ifndef SLAVE_TYPE
#define SLAVE_TYPE 1
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME "PjonSlaveTest"
#endif

// PJON instance
PJON<SoftwareBitBang> bus(SLAVE_ID);

// Message types (same as com-prot)
#define MSG_HEARTBEAT 0x03
#define MSG_COMMAND   0x04

// Command types
#define CMD_LED_CONTROL    0x10
#define CMD_TEMP_REQUEST   0x20
#define CMD_CUSTOM         0x30

// LED control
constexpr uint8_t pin_led{LED_BUILTIN};
bool ledBlinking = false;
unsigned long blinkStartTime = 0;

// Heartbeat timing
unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 1000; // 1 second

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
    
    // Handle commands
    if (length >= 3 && payload[0] == MSG_COMMAND) {
        uint8_t targetType = payload[1];
        uint8_t command = payload[2];
        
        // Check if this command is for us
        bool forMe = (targetType == 0) || (targetType == SLAVE_TYPE);
        
        if (forMe) {
            Serial.printf("Processing command 0x%02X (target type %d)\n", command, targetType);
            WebSerial.printf("Command 0x%02X received\n", command);
            
            switch (command) {
                case CMD_LED_CONTROL:
                    if (length >= 4) {
                        bool ledState = payload[3];
                        digitalWrite(pin_led, ledState ? LOW : HIGH); // LED is inverted
                        Serial.printf("LED turned %s\n", ledState ? "ON" : "OFF");
                        WebSerial.printf("LED: %s\n", ledState ? "ON" : "OFF");
                        WebSerial.flush();
                    }
                    break;
                    
                case CMD_TEMP_REQUEST:
                    {
                        // Simulate temperature reading
                        float temperature = 20.0 + (millis() % 10000) / 1000.0;
                        Serial.printf("Temperature request - sending %.2f°C\n", temperature);
                        WebSerial.printf("Temp: %.2f°C\n", temperature);
                        
                        // Send response back
                        uint8_t response[6];
                        response[0] = MSG_COMMAND;
                        response[1] = 0; // Response (unicast)
                        memcpy(&response[2], &temperature, sizeof(temperature));
                        
                        uint16_t result = bus.send(1, response, sizeof(response)); // Send to master (ID=1)
                        Serial.printf("Temperature response sent: %s\n", (result == PJON_ACK) ? "SUCCESS" : "FAILED");
                        WebSerial.flush();
                    }
                    break;
                    
                case CMD_CUSTOM:
                    Serial.printf("Custom command with %d data bytes\n", length - 3);
                    WebSerial.printf("Custom cmd (%d bytes)\n", length - 3);
                    
                    if (length > 3) {
                        Serial.print("Data: ");
                        WebSerial.print("Data: ");
                        for (uint16_t i = 3; i < length; i++) {
                            Serial.printf("0x%02X ", payload[i]);
                            WebSerial.printf("0x%02X ", payload[i]);
                        }
                        Serial.println();
                        WebSerial.println();
                    }
                    
                    // Start LED blinking for 5 seconds
                    ledBlinking = true;
                    blinkStartTime = millis();
                    WebSerial.flush();
                    break;
                    
                default:
                    Serial.printf("Unknown command: 0x%02X\n", command);
                    WebSerial.printf("Unknown cmd: 0x%02X\n", command);
                    WebSerial.flush();
                    break;
            }
        } else {
            WebSerial.printf("Command not for me (target type %d, my type %d)\n", targetType, SLAVE_TYPE);
        }
    }
}

void sendHeartbeat() {
    uint8_t heartbeat[3] = {MSG_HEARTBEAT, SLAVE_ID, SLAVE_TYPE};
    
    WebSerial.printf("[TX] Heartbeat: ID=%d, Type=%d\n", SLAVE_ID, SLAVE_TYPE);
    
    uint16_t result = bus.send(1, heartbeat, sizeof(heartbeat)); // Send to master (ID=1)
    
    // Only log failed heartbeats to reduce spam
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

void setup() {
    Serial.begin(115200);
    Serial.println("Direct PJON Slave Test");
    Serial.printf("Slave ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    
    pinMode(pin_led, OUTPUT);
    digitalWrite(pin_led, HIGH); // Turn off LED initially (inverted)
    
    // Connect to WiFi and setup OTA
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    setupOTA(-1, DEVICE_NAME);
    setupWebSerial(DEVICE_NAME);
    
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
    
    Serial.println("PJON Slave initialized");
    WebSerial.printf("Slave ID: %d, Type: %d ready\n", SLAVE_ID, SLAVE_TYPE);
    WebSerial.println("Listening for commands...");
    WebSerial.flush();
    
    // Send initial heartbeat
    sendHeartbeat();
    lastHeartbeat = millis();
}

void loop() {
    handleOTA();
    bus.update();
    bus.receive();
    
    // Send heartbeat
    if (millis() - lastHeartbeat > heartbeatInterval) {
        sendHeartbeat();
        lastHeartbeat = millis();
    }
    
    // Handle LED blinking
    blinkLed();
    
    // Status indication
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 30000) { // Every 30 seconds
        Serial.println("Slave alive and listening");
        WebSerial.println("Slave alive");
        WebSerial.flush();
        lastStatus = millis();
    }
    
}
