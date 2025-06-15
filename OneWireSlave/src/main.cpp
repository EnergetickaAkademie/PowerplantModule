/*
 *    OneWire Slave Device - DS18B20 Temperature Sensor Emulation
 *    Based on OneWireHub DS18B20_thermometer example
 *
 *    Tested with:
 *    - https://github.com/PaulStoffregen/OneWire --> DS18x20-Example
 *    - DS9490R-OneWire-Host as master device
 */

#include "DS18B20.h" // Digital Thermometer, 12bit
#include "OneWireHub.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include <ota.h>
#include <WebSerial.h>

constexpr uint8_t pin_led{LED_BUILTIN};
constexpr uint8_t pin_onewire{D1}; 

auto hub = OneWireHub(pin_onewire);
auto ds18b20 = DS18B20(DS18B20::family_code, 0x00, 0x00, 0xB2, 0x18, 0xDA, 0x00); // DS18B20: 9-12bit, -55 - +85 degC

bool blinking(void);

void setup()
{
    Serial.begin(115200); // Serial at 115200 baud    Serial.println("OneWire-Hub DS18B20 Temperature-Sensor with OTA");
    Serial.flush();
    Serial.println("Booting");
    
    pinMode(pin_led, OUTPUT);
    
    connectWifi(SECRET_SSID, SECRET_PASSWORD);
    
    setupOTA(-1, "OneWireSlave");
    setupWebSerial("OneWireSlave");

    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Setup OneWire
    hub.attach(ds18b20);    
    // Set initial temperature
    ds18b20.setTemperature(int8_t(21));

    Serial.println("config done");
}

void loop()
{
    // Handle OTA updates
    handleOTA();
    
    // following function must be called periodically
    hub.poll();
    
    // this part is just for debugging (USE_SERIAL_DEBUG in OneWire.h must be enabled for output)
    if (hub.hasError()) {
        hub.printError();
        WebSerial.println("OneWire Hub Error detected!");
    }

    // Blink triggers the state-change
    if (blinking())
    {
        // Set temp
        static float temperature = 20.0;
        temperature += 0.1;
        if (temperature > 30.0) temperature = 20.0;
        ds18b20.setTemperature(temperature);
        
        // Send debug info to both Serial and WebSerial
        Serial.println(temperature);
        WebSerial.print("Temperature set to: ");
        WebSerial.println(temperature);
    }
}

bool blinking(void)
{
    constexpr uint32_t interval   = 1000;     // interval at which to blink (milliseconds)
    static uint32_t    nextMillis = millis(); // will store next time LED will updated

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