/*
 *    PJON Temperature Sensor Slave
 *    Responds to temperature requests from master via PJON protocol
 *    More stable and reliable than OneWire communication
 *
 *    PJON Features:
 *    - Robust error handling and acknowledgments
 *    - Multi-master support
 *    - Better timing tolerance
 *    - Collision detection and retry logic
 */

#define PJON_INCLUDE_SWBB // Include SoftwareBitBang strategy
#include <PJONSoftwareBitBang.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include <ota.h>
#include <WebSerial.h>

// PJON Error codes (in case not defined in library)
#ifndef PJON_ACK
#define PJON_ACK        6
#endif
#ifndef PJON_NAK
#define PJON_NAK        21
#endif
#ifndef PJON_BUSY
#define PJON_BUSY       666
#endif
#ifndef PJON_TX_FAIL
#define PJON_TX_FAIL    65535
#endif
#ifndef PJON_FAIL
#define PJON_FAIL       65535
#endif

// PJON Configuration
#define PJON_PIN D1

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

#define MASTER_ID 1
#define HEARTBEAT_INTERVAL 1000  // Send heartbeat every 1 second

// PJON instance using SoftwareBitBang strategy
PJON<SoftwareBitBang> bus(SLAVE_ID);

// LED
constexpr uint8_t pin_led{LED_BUILTIN};

// Message types
enum MessageType {
    HEARTBEAT = 0x03
};

// Function declarations
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
void sendHeartbeat();
bool blinking(void);

void setup()
{
    Serial.begin(115200);
    Serial.println("PJON Temperature Sensor Slave");
    Serial.flush();
    Serial.println("Booting");
    
    pinMode(pin_led, OUTPUT);
      // Connect to WiFi and setup OTA
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    setupOTA(-1, DEVICE_NAME);
    setupWebSerial(DEVICE_NAME);

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());      // Initialize PJON
    bus.strategy.set_pin(PJON_PIN);
    bus.set_receiver(receiver_function);      // Configure PJON for reliable communication
    bus.set_acknowledge(false);          // Disable acknowledgments
    bus.set_crc_32(true);               // Enable 32-bit CRC for better error detection
    bus.set_packet_auto_deletion(true); // Auto-delete delivered packets
    
    bus.begin();
      Serial.print("PJON Slave initialized with ID: ");
    Serial.println(SLAVE_ID);
    Serial.print("Slave Type: ");
    Serial.println(SLAVE_TYPE);    Serial.print("Listening on pin: ");
    Serial.println(PJON_PIN);
    Serial.println("ACK disabled, CRC-32 enabled");
    Serial.print("Heartbeat interval: ");
    Serial.print(HEARTBEAT_INTERVAL);
    Serial.println("ms");
    
    WebSerial.print("PJON Slave ID: ");
    WebSerial.println(SLAVE_ID);
    WebSerial.print("Type: ");
    WebSerial.println(SLAVE_TYPE);
    WebSerial.print("Pin: ");
    WebSerial.println(PJON_PIN);
    WebSerial.print("Heartbeat: ");
    WebSerial.print(HEARTBEAT_INTERVAL);
    WebSerial.println("ms");
    
    Serial.println("Setup complete - will send heartbeats to master");
}

void loop()
{
    // Handle OTA updates
    handleOTA();
      // Update PJON (handle incoming messages)
    bus.update();
    bus.receive();
    
    // Send heartbeat every second
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
                WebSerial.println("Slave device alive and blinking");

        lastHeartbeat = millis();
    }
    
    // Small delay for PJON timing stability
    delay(10);
    
    // Blink LED periodically 
    if (blinking()) {
        WebSerial.flush();
    }
}

// PJON message receiver function
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
    if (length < 1) return;
    
    uint8_t messageType = payload[0];
    
    Serial.print("Received message from ID ");
    Serial.print(packet_info.tx.id);
    Serial.print(", type: 0x");
    Serial.println(messageType, HEX);
    
    WebSerial.print("Message from ID ");
    WebSerial.print(packet_info.tx.id);
    WebSerial.print(", type: 0x");
    WebSerial.println(messageType, HEX);
    WebSerial.flush();
    
    // Currently we only send heartbeats, no incoming message handling needed
    Serial.print("Unknown/unsupported message type: 0x");
    Serial.println(messageType, HEX);
}

void sendHeartbeat()
{
    // Prepare heartbeat message: messageType + slaveId + slaveType
    uint8_t heartbeat[3] = {HEARTBEAT, SLAVE_ID, SLAVE_TYPE};
    
    // Send heartbeat without checking result (as requested)
    bus.send(MASTER_ID, heartbeat, sizeof(heartbeat));
    
    Serial.print("Heartbeat sent - ID:");
    Serial.print(SLAVE_ID);
    Serial.print(", Type:");
    Serial.println(SLAVE_TYPE);
}

bool blinking(void)
{
    constexpr uint32_t interval = 2000;     // interval at which to blink (milliseconds)
    static uint32_t nextMillis = millis(); // will store next time LED will updated

    if (millis() > nextMillis)
    {
        nextMillis += interval;        // save the next time you blinked the LED
        static uint8_t ledState = LOW; // ledState used to set the LED
        if (ledState == LOW) ledState = HIGH;
        else ledState = LOW;
        digitalWrite(pin_led, ledState);
        return 1;
    }
    return 0;
}