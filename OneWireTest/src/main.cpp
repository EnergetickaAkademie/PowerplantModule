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

constexpr uint8_t pin_led{LED_BUILTIN};
constexpr uint8_t pin_onewire{6};  // GPIO5 (D1 on D1 Mini)

auto hub = OneWireHub(pin_onewire);
auto ds18b20 = DS18B20(DS18B20::family_code, 0x00, 0x00, 0xB2, 0x18, 0xDA, 0x00); // DS18B20: 9-12bit, -55 - +85 degC

bool blinking(void);

void setup()
{
    Serial.begin(9600);
    Serial.println("OneWire-Hub DS18B20 Temperature-Sensor");
    Serial.flush();

    pinMode(pin_led, OUTPUT);

    // Setup OneWire
    hub.attach(ds18b20);    // Set initial temperature
    ds18b20.setTemperature(int8_t(21));

    Serial.println("config done");
}

void loop()
{
    // following function must be called periodically
    hub.poll();
    // this part is just for debugging (USE_SERIAL_DEBUG in OneWire.h must be enabled for output)
    if (hub.hasError()) hub.printError();

    // Blink triggers the state-change
    if (blinking())
    {
        // Set temp
        static float temperature = 20.0;
        temperature += 0.1;
        if (temperature > 30.0) temperature = 20.0;
        ds18b20.setTemperature(temperature);
        Serial.println(temperature);
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