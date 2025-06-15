/**
 * OneWire Protocol Configuration Examples
 * 
 * This file contains example configurations for different use cases.
 * Copy the desired configuration to your main.cpp file.
 */

// =============================================================================
// EXAMPLE 1: Basic Master Configuration
// =============================================================================
/*
#define ONEWIRE_PIN 2
#define LED_PIN LED_BUILTIN
#define DEVICE_MODE_MASTER true
#define HEARTBEAT_INTERVAL 2000  // 2 seconds

// Use this for a single master controlling multiple slaves
*/

// =============================================================================
// EXAMPLE 2: Basic Slave Configuration  
// =============================================================================
/*
#define ONEWIRE_PIN 2
#define LED_PIN LED_BUILTIN
#define DEVICE_MODE_MASTER false
#define HEARTBEAT_INTERVAL 2000  // Not used in slave mode

// Use this for slave devices that respond to master heartbeat
*/

// =============================================================================
// EXAMPLE 3: Fast Heartbeat Configuration
// =============================================================================
/*
#define ONEWIRE_PIN 2
#define LED_PIN LED_BUILTIN
#define DEVICE_MODE_MASTER true
#define HEARTBEAT_INTERVAL 500   // 0.5 seconds for faster response

// Use this for applications requiring quick heartbeat detection
*/

// =============================================================================
// EXAMPLE 4: Custom Pin Configuration (if LED conflicts with OneWire)
// =============================================================================
/*
#define ONEWIRE_PIN 2       // D4 on D1 Mini
#define LED_PIN 16          // D0 on D1 Mini (separate from OneWire)
#define DEVICE_MODE_MASTER true
#define HEARTBEAT_INTERVAL 2000

// Use this if you need OneWire and LED on different pins
*/

// =============================================================================
// EXAMPLE 5: Multiple OneWire Bus Configuration
// =============================================================================
/*
#define ONEWIRE_PIN_1 2     // First bus
#define ONEWIRE_PIN_2 14    // Second bus (D5 on D1 Mini)
#define LED_PIN LED_BUILTIN
#define DEVICE_MODE_MASTER true
#define HEARTBEAT_INTERVAL 2000

// Use this for master controlling multiple independent OneWire buses
// Note: This requires modifications to the main code
*/

// =============================================================================
// Pin Reference for D1 Mini (ESP8266)
// =============================================================================
/*
GPIO    Pin Label   Notes
0       D3          Boot mode selection, avoid using as output
1       TX          Serial TX, avoid if using Serial
2       D4          Built-in LED (inverted), good for OneWire
3       RX          Serial RX, avoid if using Serial  
4       D2          SDA for I2C
5       D1          SCL for I2C
12      D6          MISO for SPI
13      D7          MOSI for SPI
14      D5          SCK for SPI
15      D8          CS for SPI, pulled low on boot
16      D0          Deep sleep wake, no interrupts
*/
