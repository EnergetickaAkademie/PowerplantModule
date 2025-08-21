/*
 * StarWire Slave (ESP8266)
 * Host sends only [type, cmd4], no payload.
 * Examples:
 *   0x01 = ON, 0x02 = OFF (for NUCLEAR/COAL atomizer)
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
#define SLAVE_ID 10
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
bool data_recieved = false;

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
#else
    // All other powerplant types use atomizer
    atomizer = factory.createAtomizer(D2);
#endif
}

// ---------- Loop ----------
void loop()
{
    factory.update();

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
