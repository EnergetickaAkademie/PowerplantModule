/*
 *    PJON Temperature Sensor Slave
 *    Using Com-Prot Library for simplified communication
 *    More stable and reliable than OneWire communication
 *
 *    Com-Prot Features:
 *    - Automatic heartbeat management
 *    - Command handler registration
 *    - Robust error handling and acknowledgments
 *    - Multi-master support
 *    - Better timing tolerance
 *    - Collision detection and retry logic
 */

#include <Arduino.h>
#include <com-prot.h>
#include "PeripheralFactory.h"
#define DEBUG_MODE

// Debug mode - uncomment to enable serial prints
// #define DEBUG_MODE

#ifdef DEBUG_MODE
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(x, ...) Serial.printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x, ...)
#endif

// Configuration from build flags (set in platformio.ini)
#ifndef SLAVE_ID
#define SLAVE_ID 10 // Default fallback
#endif

#ifndef SLAVE_TYPE
#define SLAVE_TYPE 7 // Default fallback
#endif

#ifndef DEVICE_NAME
#define DEVICE_NAME "PjonSlave" // Default fallback
#endif
#define OTA_MODE_ENABLED
// --- Powerplant type identifiers (match platformio.ini) ---
#define TYPE_PHOTOVOLTAIC 1
#define TYPE_WIND 2
#define TYPE_NUCLEAR 3
#define TYPE_GAS 4
#define TYPE_HYDRO 5
#define TYPE_HYDRO_STORAGE 6
#define TYPE_COAL 7
#define TYPE_BATTERY 8

// Restrict this firmware to supported types only
#if (SLAVE_TYPE != TYPE_BATTERY) && (SLAVE_TYPE != TYPE_NUCLEAR) && (SLAVE_TYPE != TYPE_COAL)
#error "Unsupported SLAVE_TYPE for this firmware. Supported: BATTERY (8), NUCLEAR (3), or COAL (7)."
#endif

// Create slave instance
ComProtSlave slave(SLAVE_ID, SLAVE_TYPE, D1); // Use build flags for ID and type

bool otaMode = false;      // Flag to indicate if OTA mode is enabled
PeripheralFactory factory; // Create a factory instance for peripherals

// Peripherals
Atomizer *atomizer = nullptr; // Pointer to atomizer peripheral
RGBLED *batteryLed = nullptr; // Pointer to RGB LED (BATTERY type)

// OTA-related includes and declarations (only when needed)
#ifdef OTA_MODE_ENABLED
#include <ota.h>
#include "secrets.h"
#include <ESP8266WiFi.h>
#ifdef DEBUG_MODE_WEB
#define DEBUG_WEB_PRINT(x) WebSerial.print(x)
#define DEBUG_WEB_PRINTLN(x) WebSerial.println(x)
#define DEBUG_WEB_PRINTF(x, ...) WebSerial.printf(x, __VA_ARGS__)
#define DEBUG_WEB_FLUSH() WebSerial.flush()
#else
#define DEBUG_WEB_PRINT(x)
#define DEBUG_WEB_PRINTLN(x)
#define DEBUG_WEB_PRINTF(x, ...)
#define DEBUG_WEB_FLUSH()
#endif
#else
#define DEBUG_WEB_PRINT(x)
#define DEBUG_WEB_PRINTLN(x)
#define DEBUG_WEB_PRINTF(x, ...)
#define DEBUG_WEB_FLUSH()
#endif

void handleMistyCommand(uint8_t *data, uint16_t length, uint8_t senderId)
{
    DEBUG_WEB_PRINTF("Custom command from master %d, data length: %d\n", senderId, length);
    DEBUG_PRINTF("Custom command from master %d, data length: %d\n", senderId, length);
    
    bool mistyCommand = false;
    if (length > 0)
    {
        mistyCommand = (data[0] == 1);
    }

    DEBUG_WEB_PRINTF("Misty command received: %s current state: %s XORED result: %s\n",
                     mistyCommand ? "true" : "false",
                     atomizer->getTargetState() ? "Active" : "Inactive",
                     (mistyCommand != atomizer->getTargetState()) ? "true" : "false");

    if (mistyCommand != atomizer->getTargetState())
    {
        atomizer->toggle();
        DEBUG_WEB_PRINTF("Atomizer toggled, new state: %s\n",
                         atomizer->getTargetState() ? "Active" : "Inactive");
    }
}

// --- BATTERY command (0x20): set battery mode and color on RGB LED at D2 ---
// data[0] values:
//   1 = Charging   -> RED
//   0 = Idle       -> ORANGE
//   2 = Discharging-> GREEN
static void handleBatteryModeCommand(uint8_t *data, uint16_t length, uint8_t senderId)
{
    DEBUG_WEB_PRINTF("Battery mode cmd from master %d, len: %d\n", senderId, length);
    if (length < 1 || batteryLed == nullptr)
    {
        DEBUG_WEB_PRINTLN("Invalid battery command or LED not initialized");
        return;
    }

    const uint8_t mode = data[0];
    switch (mode)
    {
    case 1: // Charging -> RED
        batteryLed->setColor(255, 0, 0);
        DEBUG_WEB_PRINTLN("Battery: Charging -> RED");
        DEBUG_PRINTLN("Battery: Charging -> RED");
        break;
    case 0:                                // Idle -> ORANGE
        batteryLed->setColor(255, 140, 0); // dark orange
        DEBUG_WEB_PRINTLN("Battery: Idle -> ORANGE");
        DEBUG_PRINTLN("Battery: Idle -> ORANGE");
        break;
    case 2: // Discharging -> GREEN
        batteryLed->setColor(0, 255, 0);
        DEBUG_WEB_PRINTLN("Battery: Discharging -> GREEN");
        DEBUG_PRINTLN("Battery: Discharging -> GREEN");
        break;
    default:
        DEBUG_WEB_PRINTF("Battery: Unknown mode %u (no change)\n", mode);
        DEBUG_PRINTF("Battery: Unknown mode %u (no change)\n", mode);
        return;
    }
    batteryLed->show(); // request immediate show; factory.update() will handle it
}

void switchAtomizer()
{
    if (atomizer)
    {
        atomizer->toggle();
    }
}

void setup()
{
    Serial.begin(115200);
    DEBUG_PRINTLN("PJON Temperature Sensor Slave with Com-Prot");
    Serial.flush();
    DEBUG_PRINTLN("Booting");
    pinMode(D0, INPUT_PULLUP); // HIGH by default
    
    // Check if D0 is low to enable OTA mode
    otaMode = !digitalRead(D0); // LOW = OTA mode enabled

#ifdef OTA_MODE_ENABLED
    // Connect to WiFi and setup OTA only if OTA mode is enabled
    if (otaMode)
    {
        connectWifi(SECRET_SSID, SECRET_PASSWORD);
        setupOTA(-1, DEVICE_NAME);
        setupWebSerial(DEVICE_NAME);
        
        DEBUG_PRINTLN("OTA Mode enabled - D0 is LOW");
        DEBUG_WEB_PRINTLN("OTA Mode enabled - D0 is LOW");
        DEBUG_WEB_FLUSH();

        // In OTA mode, we'll wait for updates and not proceed with normal operation
        while (true)
        {
            handleOTA();
            delay(100);
        }
    }
#endif

    DEBUG_PRINTLN("Normal mode - D0 is HIGH or OTA disabled");
    DEBUG_WEB_PRINTLN("Normal mode - D0 is HIGH or OTA disabled");
    DEBUG_WEB_FLUSH();

    DEBUG_PRINTLN("Ready");
#ifdef OTA_MODE_ENABLED
    if (otaMode) {
        DEBUG_PRINT("IP address: ");
        DEBUG_PRINTLN(WiFi.localIP());
    }
#endif
    // Register command handlers
#if SLAVE_TYPE == TYPE_BATTERY
    slave.setCommandHandler(0x20, handleBatteryModeCommand); // Battery mode command
#elif (SLAVE_TYPE == TYPE_NUCLEAR) || (SLAVE_TYPE == TYPE_COAL)
    slave.setCommandHandler(0x10, handleMistyCommand); // Custom command for nuclear and coal
#endif

    // Initialize the slave
    slave.begin();

    DEBUG_PRINTF("Com-Prot Slave initialized - ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    DEBUG_WEB_PRINTF("Slave ID: %d, Type: %d\n", SLAVE_ID, SLAVE_TYPE);
    DEBUG_WEB_PRINTLN("Command handlers registered:");
#if SLAVE_TYPE == TYPE_BATTERY
    DEBUG_WEB_PRINTLN("  0x20 - battery mode (0=idle,1=charging,2=discharging)");
#elif (SLAVE_TYPE == TYPE_NUCLEAR) || (SLAVE_TYPE == TYPE_COAL)
    DEBUG_WEB_PRINTLN("  0x10 - custom misty command");
#endif
    DEBUG_WEB_PRINTLN("Debug receive handler enabled");
    DEBUG_WEB_FLUSH();

    DEBUG_PRINTLN("Setup complete - sending heartbeats to master");
#if SLAVE_TYPE == TYPE_BATTERY
    batteryLed = factory.createRGBLED(D2, 1); // WS2812B on D2, single pixel
    batteryLed->setBrightness(64);            // moderate brightness
    batteryLed->setColor(255, 140, 0);        // default to idle/orange
    batteryLed->show();
#elif (SLAVE_TYPE == TYPE_NUCLEAR) || (SLAVE_TYPE == TYPE_COAL)
    atomizer = factory.createAtomizer(D2); // Atomizer on D2
#endif
    // factory.createPeriodic(1000, switchAtomizer); // Create periodic task to switch atomizer every second
}

void loop()
{
    factory.update(); // Update all peripherals
    
#ifdef OTA_MODE_ENABLED
    // Handle OTA updates only if OTA mode is enabled
    if (otaMode)
        handleOTA();
#endif

    // Update slave (handles incoming messages and heartbeats)
    slave.update();

}