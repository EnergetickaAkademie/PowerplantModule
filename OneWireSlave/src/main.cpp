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
#define SLAVE_ID 10  // Change this for each slave device (10, 11, 12, etc.)
#define MASTER_ID 1

// PJON instance using SoftwareBitBang strategy
PJON<SoftwareBitBang> bus(SLAVE_ID);

// LED
constexpr uint8_t pin_led{LED_BUILTIN};

// Message types
enum MessageType {
    HELLO_REQUEST = 0x01,
    HELLO_RESPONSE = 0x02,
    HEARTBEAT = 0x03
};

// Function declarations
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
void sendHelloResponse(uint8_t receiver_id);
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
    setupOTA(-1, "PjonSlave");
    setupWebSerial("PjonSlave");

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
      // Initialize PJON
    bus.strategy.set_pin(PJON_PIN);
    bus.set_receiver(receiver_function);
      // Configure PJON for reliable communication
    bus.set_acknowledge(true);           // Enable acknowledgments
    bus.set_crc_32(true);               // Enable 32-bit CRC for better error detection
    bus.set_packet_auto_deletion(true); // Auto-delete delivered packets
    
    // Set timing for better ESP8266 compatibility
    bus.strategy.set_read_delay(4);      // Microseconds delay for bit reading
    bus.strategy.set_bit_spacer(56);     // Microseconds between bits
    
    bus.begin();
    
    Serial.print("PJON Slave initialized with ID: ");
    Serial.println(SLAVE_ID);
    Serial.print("Listening on pin: ");
    Serial.println(PJON_PIN);
    Serial.println("ACK enabled, CRC-32 enabled");
    
    WebSerial.print("PJON Slave ID: ");
    WebSerial.println(SLAVE_ID);
    WebSerial.print("Pin: ");
    WebSerial.println(PJON_PIN);
    
    Serial.println("Setup complete");
}

void loop()
{
    // Handle OTA updates
    handleOTA();
      // Update PJON (handle incoming messages)
    bus.update();
    bus.receive();
    
    // Small delay for PJON timing stability
    delay(10);
    
    // Blink LED periodically 
    if (blinking()) {
        WebSerial.println("Slave device alive and blinking");
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
    
    // Use flush to ensure WebSerial output is safe
    WebSerial.print("Message from ID ");
    WebSerial.print(packet_info.tx.id);
    WebSerial.print(", type: 0x");
    WebSerial.println(messageType, HEX);
    WebSerial.flush();
    
    switch (messageType) {        case HELLO_REQUEST:
            Serial.println("Hello request received");
            WebSerial.println("Hello request received");
            WebSerial.flush();
            sendHelloResponse(packet_info.tx.id);
            break;
            
        case HEARTBEAT:
            Serial.println("Heartbeat received");
            WebSerial.println("Heartbeat received");
            WebSerial.flush();
            // Blink LED to acknowledge heartbeat
            digitalWrite(pin_led, HIGH);
            delay(100);
            digitalWrite(pin_led, LOW);
            break;
            
        default:
            Serial.print("Unknown message type: 0x");
            Serial.println(messageType, HEX);
            WebSerial.print("Unknown message type: 0x");
            WebSerial.println(messageType, HEX);
            WebSerial.flush();
            break;
    }
}

void sendHelloResponse(uint8_t receiver_id)
{
    // Prepare hello response message
    char helloMsg[] = "Hello World from Slave!";
    size_t msgLen = strlen(helloMsg);
    
    // Calculate actual response size (message type + string length)
    size_t responseSize = 1 + msgLen;
    uint8_t response[responseSize];
    
    response[0] = HELLO_RESPONSE;
    
    // Copy hello message
    memcpy(&response[1], helloMsg, msgLen);
      // Send response with actual size (not fixed 25 bytes)
    uint8_t result = bus.send(receiver_id, response, responseSize);
    
    if (result == PJON_ACK) {
        Serial.println("Hello response sent successfully");
        WebSerial.println("Hello response sent successfully");
        WebSerial.flush();
    } else {
        Serial.print("Failed to send hello response, error code: ");
        Serial.println(result, HEX);
        WebSerial.print("Failed to send hello response, error code: 0x");
        WebSerial.println(result, HEX);
        
        // Print common PJON error codes for debugging
        if (result == PJON_FAIL) {
            Serial.println("Error: PJON_FAIL - General failure");
            WebSerial.println("Error: PJON_FAIL - General failure");
        } else if (result == PJON_BUSY) {
            Serial.println("Error: PJON_BUSY - Bus is busy");
            WebSerial.println("Error: PJON_BUSY - Bus is busy");
        } else if (result == PJON_TX_FAIL) {
            Serial.println("Error: PJON_TX_FAIL - Transmission failed");
            WebSerial.println("Error: PJON_TX_FAIL - Transmission failed");
        }
        WebSerial.flush();
    }
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