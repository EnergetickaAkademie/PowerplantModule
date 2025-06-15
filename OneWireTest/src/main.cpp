#include <Arduino.h>
#include <OneWire.h>

// Pin definitions
#define ONEWIRE_PIN 2        // GPIO2 (D4 on D1 Mini) - OneWire bus pin
#define LED_PIN LED_BUILTIN  // Built-in LED for status indication

// Configuration - Change these to set device mode and ID
#define DEVICE_MODE_MASTER true  // Set to false for slave mode
#define SLAVE_ID 1               // Only used in slave mode (1, 2, 3, etc.)

// Protocol constants
#define HEARTBEAT_COMMAND 0xAA
#define HEARTBEAT_INTERVAL 3000  // 3 seconds between heartbeats
#define MAX_SLAVES 5             // Maximum number of slaves to cycle through

// Global variables
OneWire oneWire(ONEWIRE_PIN);
unsigned long lastHeartbeat = 0;
uint8_t currentSlaveId = 1;      // Current slave ID being addressed

class OneWireHeartbeat {
public:
  // Master: Send heartbeat with specific slave ID
  void sendHeartbeatToSlave(uint8_t slaveId) {
    Serial.print("=== Sending Heartbeat to Slave ID: ");
    Serial.print(slaveId);
    Serial.println(" ===");
    
    // Reset and check for presence
    if (!oneWire.reset()) {
      Serial.println("No devices found on bus");
      return;
    }
    
    // Skip ROM command (address all devices)
    oneWire.write(0xCC);
    
    // Send heartbeat command
    oneWire.write(HEARTBEAT_COMMAND);
    
    // Send slave ID
    oneWire.write(slaveId);
    
    Serial.print("Heartbeat sent with ID: ");
    Serial.println(slaveId);
    
    // Blink LED to show master activity
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }
  
  // Slave: Listen for heartbeat and check if it's for this device
  bool listenForHeartbeat(uint8_t mySlaveId) {
    // Check if there's a reset pulse (start of communication)
    if (oneWire.reset()) {
      delay(1); // Small delay to ensure master completes setup
      
      // Try to read the command sequence
      uint8_t command = oneWire.read();
      
      if (command == 0xCC) { // Skip ROM command received
        uint8_t heartbeatCmd = oneWire.read();
        
        if (heartbeatCmd == HEARTBEAT_COMMAND) {
          uint8_t receivedId = oneWire.read();
          
          Serial.print("Received heartbeat for ID: ");
          Serial.print(receivedId);
          Serial.print(" (My ID: ");
          Serial.print(mySlaveId);
          Serial.println(")");
          
          if (receivedId == mySlaveId) {
            Serial.println("*** This heartbeat is for ME! ***");
            return true;
          } else {
            Serial.println("Heartbeat for different slave, ignoring");
          }
        }
      }
    }
    return false;
  }
  
  // Blink LED the number of times equal to slave ID
  void blinkForId(uint8_t slaveId) {
    Serial.print("Blinking ");
    Serial.print(slaveId);
    Serial.println(" times for my ID");
    
    for (uint8_t i = 0; i < slaveId; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);
      delay(300);
    }
    
    // Longer pause after blinking sequence
    delay(1000);
  }
};

OneWireHeartbeat heartbeat;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== OneWire ESP8266 Master-Slave Communication ===");
  Serial.println("Using OneWire library for reliable communication");
  
  // Initialize LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  if (DEVICE_MODE_MASTER) {
    Serial.println("*** DEVICE CONFIGURED AS MASTER ***");
    Serial.print("Will send heartbeats to slaves 1-");
    Serial.println(MAX_SLAVES);
    Serial.print("Heartbeat interval: ");
    Serial.print(HEARTBEAT_INTERVAL);
    Serial.println(" ms");
    
  } else {
    Serial.println("*** DEVICE CONFIGURED AS SLAVE ***");
    Serial.print("My Slave ID: ");
    Serial.println(SLAVE_ID);
    Serial.print("Will blink ");
    Serial.print(SLAVE_ID);
    Serial.println(" times when my ID is called");
  }
  
  Serial.print("OneWire bus pin: GPIO");
  Serial.println(ONEWIRE_PIN);
  Serial.println("Waiting for communication...");
  Serial.println();
}

void loop() {
  if (DEVICE_MODE_MASTER) {
    // Master mode: Send heartbeat to each slave in sequence
    unsigned long currentTime = millis();
    
    if (currentTime - lastHeartbeat >= HEARTBEAT_INTERVAL) {
      // Send heartbeat to current slave ID
      heartbeat.sendHeartbeatToSlave(currentSlaveId);
      
      // Move to next slave (cycle through 1 to MAX_SLAVES)
      currentSlaveId++;
      if (currentSlaveId > MAX_SLAVES) {
        currentSlaveId = 1;
        Serial.println("--- Completed full cycle, starting over ---");
        Serial.println();
      }
      
      lastHeartbeat = currentTime;
    }
    
    delay(100);
    
  } else {
    // Slave mode: Listen for heartbeat with my ID
    if (heartbeat.listenForHeartbeat(SLAVE_ID)) {
      // This heartbeat is for me! Blink my ID number of times
      heartbeat.blinkForId(SLAVE_ID);
    }
    
    delay(10); // Small delay for responsiveness
  }
}