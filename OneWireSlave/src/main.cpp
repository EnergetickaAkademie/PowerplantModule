/*
 * StarWire Slave (ESP8266)
 * Host sends only [type, cmd4], no payload.
 * Examples:// Photovoltaic specific variables
#define SOLAR_PIN A0
unsigned long lastSolarUpdate = 0;
const unsigned long SOLAR_UPDATE_INTERVAL = 100; // Update every 100ms
uint8_t solarMode = 0; // 0=idle (green), 1=half power (orange), 2=night (red)  0x01 = ON, 0x02 = OFF (for NUCLEAR/COAL atomizer)
 *   0x00 = Idle, 0x01 = Charging, 0x02 = Discharging (for BATTERY LED)
 */

#include <Arduino.h>
#include <com-prot.h>
#include "PeripheralFactory.h"
#define DEBUG_MODE

using namespace StarWire;

#ifdef DEBUG_MODE
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(x, ...) Serial.printf(x, __VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(x, ...)
#endif

#ifndef SLAVE_ID
#define SLAVE_ID 1000
#endif
#ifndef SLAVE_TYPE
#define SLAVE_TYPE 7
#endif
#ifndef DEVICE_NAME
#define DEVICE_NAME "StarWireSlave"
#endif


// Types
#define TYPE_PHOTOVOLTAIC 1
#define TYPE_WIND 2
#define TYPE_NUCLEAR 3
#define TYPE_GAS 4
#define TYPE_HYDRO 5
#define TYPE_HYDRO_STORAGE 6
#define TYPE_COAL 7
#define TYPE_BATTERY 8

#define DATA_PIN D1
#define CLK_PIN D7
#if (SLAVE_TYPE != TYPE_PHOTOVOLTAIC) && (SLAVE_TYPE != TYPE_WIND) && (SLAVE_TYPE != TYPE_NUCLEAR) && (SLAVE_TYPE != TYPE_GAS) && (SLAVE_TYPE != TYPE_HYDRO) && (SLAVE_TYPE != TYPE_HYDRO_STORAGE) && (SLAVE_TYPE != TYPE_COAL) && (SLAVE_TYPE != TYPE_BATTERY)
#error "Supported SLAVE_TYPEs: PHOTOVOLTAIC(1), WIND(2), NUCLEAR(3), GAS(4), HYDRO(5), HYDRO_STORAGE(6), COAL(7), BATTERY(8)."
#endif

ComProtSlave slave(SLAVE_ID, SLAVE_TYPE, DATA_PIN, CLK_PIN);

bool otaMode = false;
PeripheralFactory factory;

Atomizer *atomizer = nullptr; // for NUCLEAR/COAL
RGBLED *batteryLed = nullptr; // for BATTERY
RGBLED *solarLed = nullptr; // for PHOTOVOLTAIC
RGBLED *gasLed = nullptr; // for GAS
MotorSinglePin *hydroMotor = nullptr; // for HYDRO
RGBLED *hydroStorageLed = nullptr; // for HYDRO_STORAGE
MotorSinglePin *windMotor = nullptr; // for WIND

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

// 4-bit commands
static const uint8_t CMD_ON = 0x01;
static const uint8_t CMD_OFF = 0x02;
static const uint8_t BAT_IDLE = 0x03;
static const uint8_t BAT_CHARGING = 0x04;
static const uint8_t BAT_DISCHARGE = 0x05;

// Gas powerplant specific commands (expanded to 10 levels)
// We reuse the 4-bit command space (0x0 - 0xF). Codes are shared across types, so
// overlapping numeric values with other powerplants is acceptable.
// Gradient (low -> high production) per user spec:
// 1: (255, 0, 0)
// 2: (227, 28, 36)
// 3: (198, 57, 71)
// 4: (170, 85,107)
// 5: (142,113,142)
// 6: (113,142,142)
// 7: ( 85,170,107)
// 8: ( 57,198, 71)
// 9: ( 28,227, 36)
//10: (  0,255,  0)
// Level numbering kept sequential for clarity.
static const uint8_t GAS_LEVEL_1  = 0x06; // 10%  Red
static const uint8_t GAS_LEVEL_2  = 0x07; // 20%  Purple
static const uint8_t GAS_LEVEL_3  = 0x08; // 30%  Blue
static const uint8_t GAS_LEVEL_4  = 0x09; // 40%  Light Blue
static const uint8_t GAS_LEVEL_5  = 0x0A; // 50%  Cyan
static const uint8_t GAS_LEVEL_6  = 0x0B; // 60%  Teal
static const uint8_t GAS_LEVEL_7  = 0x0C; // 70%  Green
static const uint8_t GAS_LEVEL_8  = 0x0D; // 80%  Lime
static const uint8_t GAS_LEVEL_9  = 0x0E; // 90%  Yellow-Green
static const uint8_t GAS_LEVEL_10 = 0x0F; // 100% Yellow (peak)

// Hydro Storage specific commands (5 levels)
static const uint8_t HYDRO_STORAGE_LEVEL_1 = 0x0B; // Green - 100% (Discharging)
static const uint8_t HYDRO_STORAGE_LEVEL_2 = 0x0C; // Light Green - 75%
static const uint8_t HYDRO_STORAGE_LEVEL_3 = 0x0D; // Orange - 50% (Idle)
static const uint8_t HYDRO_STORAGE_LEVEL_4 = 0x0E; // Light Red - 25%
static const uint8_t HYDRO_STORAGE_LEVEL_5 = 0x0F; // Red - 0% (Charging)

bool data_recieved = false;

// Photovoltaic specific variables
#define SOLAR_PIN A0
unsigned long lastSolarUpdate = 0;
const unsigned long SOLAR_UPDATE_INTERVAL = 50; // Update every 100ms
uint8_t solarMode = 0; // 0=default green, 1=high production, 2=low production

// ---------- Handlers ----------
static void handleMisty(uint8_t cmd4, uint8_t senderId)
{
    DEBUG_PRINTF("[MISTY] cmd=0x%X from master %u\n", cmd4, senderId);
    data_recieved = true;
    if (!atomizer)
        return;

    bool wantOn = (cmd4 == CMD_ON);
    bool wantOff = (cmd4 == CMD_OFF);

    if (!wantOn && !wantOff)
    {
        DEBUG_PRINTLN("[MISTY] Unknown cmd, ignored.");
        return;
    }

    bool target = wantOn;
    if (target != atomizer->getTargetState())
    {
        atomizer->toggle();
        DEBUG_PRINTF("[MISTY] Toggled -> %s\n", atomizer->getTargetState() ? "Active" : "Inactive");
    }
}

static void handleBattery(uint8_t cmd4, uint8_t senderId)
{
    data_recieved = true;
    DEBUG_PRINTF("[BAT] cmd=0x%X from master %u\n", cmd4, senderId);
    if (!batteryLed)
    {
        DEBUG_PRINTLN("[BAT] LED not initialized");
        return;
    }

    switch (cmd4)
    {
    case BAT_CHARGING:
        batteryLed->setColor(255, 0, 0);
        DEBUG_PRINTLN("Battery: Charging -> RED");
        break;
    case BAT_IDLE:
        batteryLed->setColor(255, 140, 0);
        DEBUG_PRINTLN("Battery: Idle -> ORANGE");
        break;
    case BAT_DISCHARGE:
        batteryLed->setColor(0, 255, 0);
        DEBUG_PRINTLN("Battery: Discharging -> GREEN");
        break;
    default:
        DEBUG_PRINTF("Battery: Unknown cmd 0x%X (ignored)\n", cmd4);
        return;
    }
    batteryLed->show();
}

static void handlePhotovoltaic(uint8_t cmd4, uint8_t senderId)
{
    data_recieved = true;
    DEBUG_PRINTF("[SOLAR] cmd=0x%X from master %u\n", cmd4, senderId);
    if (!solarLed)
    {
        DEBUG_PRINTLN("[SOLAR] LED not initialized");
        return;
    }

    switch (cmd4)
    {
    case CMD_ON: // Half power mode (orange)
        solarMode = 1;
        DEBUG_PRINTLN("Solar: Half power mode (orange)");
        break;
    case CMD_OFF: // Night mode (red, no light reaction)
        solarMode = 2;
        DEBUG_PRINTLN("Solar: Night mode (red)");
        break;
    case BAT_IDLE: // Idle mode (green)
        solarMode = 0;
        DEBUG_PRINTLN("Solar: Idle mode (green)");
        break;
    default:
        DEBUG_PRINTF("Solar: Unknown cmd 0x%X (ignored)\n", cmd4);
        return;
    }
}

static void handleGas(uint8_t cmd4, uint8_t senderId)
{
    data_recieved = true;
    DEBUG_PRINTF("[GAS] cmd=0x%X from master %u\n", cmd4, senderId);
    if (!gasLed) {
        DEBUG_PRINTLN("[GAS] LED not initialized");
        return;
    }

    switch (cmd4) {
    case GAS_LEVEL_1:  gasLed->setColor(255, 0, 0);    gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 1 -> (255,0,0)"); break;
    case GAS_LEVEL_2:  gasLed->setColor(227, 28, 36);  gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 2 -> (227,28,36)"); break;
    case GAS_LEVEL_3:  gasLed->setColor(198, 57, 71);  gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 3 -> (198,57,71)"); break;
    case GAS_LEVEL_4:  gasLed->setColor(170, 85,107);  gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 4 -> (170,85,107)"); break;
    case GAS_LEVEL_5:  gasLed->setColor(142,113,142);  gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 5 -> (142,113,142)"); break;
    case GAS_LEVEL_6:  gasLed->setColor(113,142,142);  gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 6 -> (113,142,142)"); break;
    case GAS_LEVEL_7:  gasLed->setColor(85,170,107);   gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 7 -> (85,170,107)"); break;
    case GAS_LEVEL_8:  gasLed->setColor(57,198,71);    gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 8 -> (57,198,71)"); break;
    case GAS_LEVEL_9:  gasLed->setColor(28,227,36);    gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 9 -> (28,227,36)"); break;
    case GAS_LEVEL_10: gasLed->setColor(0,255,0);      gasLed->setBrightness(255); DEBUG_PRINTLN("Gas: Level 10 -> (0,255,0) MAX"); break;
    case CMD_OFF:      gasLed->setColor(0,0,0);        gasLed->setBrightness(0);   DEBUG_PRINTLN("Gas: OFF"); break;
    default: DEBUG_PRINTF("Gas: Unknown cmd 0x%X (ignored)\n", cmd4); return; }
    gasLed->show();
}

static void handleHydro(uint8_t cmd4, uint8_t senderId)
{
    DEBUG_PRINTF("[HYDRO] cmd=0x%X from master %u\n", cmd4, senderId);
    data_recieved = true;
    if (!hydroMotor)
        return;

    bool wantOn = (cmd4 == CMD_ON);
    bool wantOff = (cmd4 == CMD_OFF);

    if (!wantOn && !wantOff)
    {
        DEBUG_PRINTLN("[HYDRO] Unknown cmd, ignored.");
        return;
    }

    if (wantOn)
    {
        hydroMotor->forward(1023); // 100% speed
        DEBUG_PRINTLN("[HYDRO] Motor ON -> 100%");
    }
    else
    {
        hydroMotor->stop();
        DEBUG_PRINTLN("[HYDRO] Motor OFF");
    }
}

static void handleHydroStorage(uint8_t cmd4, uint8_t senderId)
{
    data_recieved = true;
    DEBUG_PRINTF("[HYDRO_STORAGE] cmd=0x%X from master %u\n", cmd4, senderId);
    if (!hydroStorageLed)
    {
        DEBUG_PRINTLN("[HYDRO_STORAGE] LED not initialized");
        return;
    }

    switch (cmd4)
    {
    case HYDRO_STORAGE_LEVEL_1: // Green - 100% (Full/Discharging)
        hydroStorageLed->setColor(0, 255, 0);
        hydroStorageLed->setBrightness(255);
        DEBUG_PRINTLN("Hydro Storage: Level 1 -> GREEN (100% - Full/Discharging)");
        break;
    case HYDRO_STORAGE_LEVEL_2: // Light Green - 75%
        hydroStorageLed->setColor(128, 255, 128);
        hydroStorageLed->setBrightness(192);
        DEBUG_PRINTLN("Hydro Storage: Level 2 -> LIGHT GREEN (75%)");
        break;
    case HYDRO_STORAGE_LEVEL_3: // Orange - 50% (Idle)
        hydroStorageLed->setColor(255, 140, 0);
        hydroStorageLed->setBrightness(128);
        DEBUG_PRINTLN("Hydro Storage: Level 3 -> ORANGE (50% - Idle)");
        break;
    case HYDRO_STORAGE_LEVEL_4: // Light Red - 25%
        hydroStorageLed->setColor(255, 128, 128);
        hydroStorageLed->setBrightness(96);
        DEBUG_PRINTLN("Hydro Storage: Level 4 -> LIGHT RED (25%)");
        break;
    case HYDRO_STORAGE_LEVEL_5: // Red - 0% (Empty/Charging)
        hydroStorageLed->setColor(255, 0, 0);
        hydroStorageLed->setBrightness(64);
        DEBUG_PRINTLN("Hydro Storage: Level 5 -> RED (0% - Empty/Charging)");
        break;
    case CMD_OFF: // Turn off
        hydroStorageLed->setColor(0, 0, 0);
        hydroStorageLed->setBrightness(0);
        DEBUG_PRINTLN("Hydro Storage: OFF");
        break;
    default:
        DEBUG_PRINTF("Hydro Storage: Unknown cmd 0x%X (ignored)\n", cmd4);
        return;
    }
    hydroStorageLed->show();
}

static void handleWind(uint8_t cmd4, uint8_t senderId)
{
    DEBUG_PRINTF("[WIND] cmd=0x%X from master %u\n", cmd4, senderId);
    data_recieved = true;
    if (!windMotor)
        return;

    bool wantOn = (cmd4 == CMD_ON);
    bool wantOff = (cmd4 == CMD_OFF);

    if (!wantOn && !wantOff)
    {
        DEBUG_PRINTLN("[WIND] Unknown cmd, ignored.");
        return;
    }

    if (wantOn)
    {
        windMotor->forward(50); // 50 power level as specified
        DEBUG_PRINTLN("[WIND] Motor ON -> 50 power level");
    }
    else
    {
        windMotor->stop();
        DEBUG_PRINTLN("[WIND] Motor OFF");
    }
}

// ---------- Setup ----------
void setup()
{
    Serial.begin(115200);
    DEBUG_PRINTLN("StarWire Slave booting...");
    pinMode(D0, INPUT_PULLUP);

#ifdef OTA_MODE_ENABLED
    otaMode = !digitalRead(D0); // LOW -> OTA mode
    if (otaMode)
    {
        connectWifi(SECRET_SSID, SECRET_PASSWORD);
        setupOTA(-1, DEVICE_NAME);
        setupWebSerial(DEVICE_NAME);
        DEBUG_PRINTLN("OTA Mode enabled - D0 is LOW");
        while (true)
        {
            handleOTA();
            delay(100);
        }
    }
#endif

    // Register handlers (4-bit, no payload)
#if SLAVE_TYPE == TYPE_BATTERY
    slave.setCommandHandler(BAT_IDLE, handleBattery);
    slave.setCommandHandler(BAT_CHARGING, handleBattery);
    slave.setCommandHandler(BAT_DISCHARGE, handleBattery);
#elif SLAVE_TYPE == TYPE_PHOTOVOLTAIC
    slave.setCommandHandler(CMD_ON, handlePhotovoltaic);
    slave.setCommandHandler(CMD_OFF, handlePhotovoltaic);
    slave.setCommandHandler(BAT_IDLE, handlePhotovoltaic);
#elif SLAVE_TYPE == TYPE_GAS
    slave.setCommandHandler(GAS_LEVEL_1, handleGas);
    slave.setCommandHandler(GAS_LEVEL_2, handleGas);
    slave.setCommandHandler(GAS_LEVEL_3, handleGas);
    slave.setCommandHandler(GAS_LEVEL_4, handleGas);
    slave.setCommandHandler(GAS_LEVEL_5, handleGas);
    slave.setCommandHandler(GAS_LEVEL_6, handleGas);
    slave.setCommandHandler(GAS_LEVEL_7, handleGas);
    slave.setCommandHandler(GAS_LEVEL_8, handleGas);
    slave.setCommandHandler(GAS_LEVEL_9, handleGas);
    slave.setCommandHandler(GAS_LEVEL_10, handleGas);
    slave.setCommandHandler(CMD_OFF, handleGas);
#elif SLAVE_TYPE == TYPE_HYDRO
    slave.setCommandHandler(CMD_ON, handleHydro);
    slave.setCommandHandler(CMD_OFF, handleHydro);
#elif SLAVE_TYPE == TYPE_HYDRO_STORAGE
    slave.setCommandHandler(HYDRO_STORAGE_LEVEL_1, handleHydroStorage);
    slave.setCommandHandler(HYDRO_STORAGE_LEVEL_2, handleHydroStorage);
    slave.setCommandHandler(HYDRO_STORAGE_LEVEL_3, handleHydroStorage);
    slave.setCommandHandler(HYDRO_STORAGE_LEVEL_4, handleHydroStorage);
    slave.setCommandHandler(HYDRO_STORAGE_LEVEL_5, handleHydroStorage);
    slave.setCommandHandler(CMD_OFF, handleHydroStorage);
#elif SLAVE_TYPE == TYPE_WIND
    slave.setCommandHandler(CMD_ON, handleWind);
    slave.setCommandHandler(CMD_OFF, handleWind);
#else
    // All other powerplant types use the same ON/OFF commands
    slave.setCommandHandler(CMD_ON, handleMisty);
    slave.setCommandHandler(CMD_OFF, handleMisty);
#endif

    slave.begin();

    DEBUG_PRINTF("Slave ready: ID=%u, Type=%u\n", SLAVE_ID, SLAVE_TYPE);

#if SLAVE_TYPE == TYPE_BATTERY
    batteryLed = factory.createRGBLED(D2, 1);
    batteryLed->setBrightness(64);
    batteryLed->setColor(255, 140, 0);
    batteryLed->show();
#elif SLAVE_TYPE == TYPE_PHOTOVOLTAIC
    solarLed = factory.createRGBLED(D2, 1);
    solarLed->setBrightness(32); // Start with low brightness
    solarLed->setColor(0, 255, 0); // Default green
    solarLed->show();
    pinMode(SOLAR_PIN, INPUT);
    DEBUG_PRINTLN("Solar panel initialized on A0, LED on D2");
#elif SLAVE_TYPE == TYPE_GAS
    gasLed = factory.createRGBLED(D2, 1);
    gasLed->setBrightness(255);
    gasLed->setColor(255, 0, 0); // Start at Level 1 color
    gasLed->show();
    DEBUG_PRINTLN("Gas powerplant LED initialized on D2 (10-level mode)");
#elif SLAVE_TYPE == TYPE_HYDRO
    hydroMotor = factory.createMotorSinglePin(D5, 1000); // Motor on D5, 1kHz PWM
    hydroMotor->stop(); // Start with motor off
    DEBUG_PRINTLN("Hydro motor initialized on D5");
#elif SLAVE_TYPE == TYPE_HYDRO_STORAGE
    hydroStorageLed = factory.createRGBLED(D2, 1);
    hydroStorageLed->setBrightness(128);
    hydroStorageLed->setColor(255, 140, 0); // Start with orange (50% - Idle)
    hydroStorageLed->show();
    DEBUG_PRINTLN("Hydro Storage LED initialized on D2");
#elif SLAVE_TYPE == TYPE_WIND
    windMotor = factory.createMotorSinglePin(D5, 15); // Motor on D5, 15Hz PWM as specified
    windMotor->enableSpeedup(true);
    windMotor->setSpeedupConfig(2.5f, 1000); // 2.5x multiplier, 1000ms duration
    windMotor->stop(); // Start with motor off

    DEBUG_PRINTLN("Wind motor initialized on D5 with speedup enabled");
#else
    // All other powerplant types use atomizer
    atomizer = factory.createAtomizer(D2);
#endif
}

// ---------- Loop ----------
void loop()
{
    factory.update();

#if SLAVE_TYPE == TYPE_PHOTOVOLTAIC
    // Update solar panel brightness based on A0 reading
    if (millis() - lastSolarUpdate >= SOLAR_UPDATE_INTERVAL)
    {
        lastSolarUpdate = millis();
        
        int analogValue = analogRead(SOLAR_PIN);
        float voltage = (analogValue / 1024.0) * 3.3; // Convert to voltage (0-3.3V)
        
        // Map analog value to brightness (minimum 16, maximum 255)
        uint8_t brightness = map(analogValue, 0, 1024, 16, 255);
        
        uint8_t red = 0, green = 0, blue = 0;
        
        switch (solarMode)
        {
        case 0: // Idle mode (green) - shifts to orange when A0 is low
            {
                // Green (0,255,0) to Orange (255,165,0)
                // Map analogValue: 1024 = pure green, 0 = orange
                uint8_t lightLevel = map(analogValue, 0, 1024, 0, 255);
                red = 255 - lightLevel; // More red when light is low
                green = 255; // Always full green
                blue = 0;
            }
            break;
            
        case 1: // Half power mode (orange) - shifts to red when A0 is low
            {
                // Orange (255,165,0) to Red (255,0,0)
                // Map analogValue: 1024 = orange, 0 = red
                uint8_t lightLevel = map(analogValue, 0, 1024, 0, 165);
                red = 255; // Always full red
                green = lightLevel; // Less green when light is low
                blue = 0;
            }
            break;
            
        case 2: // Night mode (red) - no reaction to light
            red = 255;
            green = 0;
            blue = 0;
            // For night mode, use fixed low brightness regardless of A0
            brightness = 32;
            break;
        }
        
        solarLed->setColor(red, green, blue);
        solarLed->setBrightness(brightness);
        solarLed->show();
        
        DEBUG_PRINTF("[SOLAR] A0=%d, V=%.2fV, Bright=%d, Mode=%d, RGB=(%d,%d,%d)\n", 
                    analogValue, voltage, brightness, solarMode, red, green, blue);
    }
#endif

#ifdef OTA_MODE_ENABLED
    if (otaMode)
        handleOTA();
#endif
#ifdef DEBUG_MODE
static long startTime = millis();
if (millis() - startTime > 1000)
{
    DEBUG_PRINTF("Looping... (uptime: %ld seconds) data_received: %s\n", (millis() / 1000), data_recieved ? "true" : "false");
    DEBUG_WEB_PRINTF("Looping... (uptime: %ld seconds) data_received: %s\n", (millis() / 1000), data_recieved ? "true" : "false");
    startTime = millis();


}
#endif

    slave.update();
}
