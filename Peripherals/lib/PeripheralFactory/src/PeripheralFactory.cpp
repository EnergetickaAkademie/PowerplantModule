#include "PeripheralFactory.h"
#include <Arduino.h>

PeripheralFactory::PeripheralFactory() {
    _peripheralCount = 0;
}

PeripheralFactory::~PeripheralFactory() {
    for (int i = 0; i < _peripheralCount; i++) {
        delete _peripherals[i];
        _peripherals[i] = nullptr;
    }
}

void PeripheralFactory::add(Peripheral* peripheral) {
    if (_peripheralCount < MAX_PERIPHERALS) {
        _peripherals[_peripheralCount] = peripheral;
        _peripheralCount++;
    } else {
        Serial.println(F("Error: PeripheralFactory is full. Cannot add more peripherals."));
        delete peripheral;
    }
}

LED* PeripheralFactory::createLed(int pin) {
    LED* led = new LED(pin);
    add(led);
    return led;
}

Motor* PeripheralFactory::createMotor(int pinIA, int pinIB) {
    Motor* motor = new Motor(pinIA, pinIB);
    add(motor);
    return motor;
}

OLEDDisplay* PeripheralFactory::createOLED(uint8_t w, uint8_t h, TwoWire *twi, int8_t rst_pin) {
    OLEDDisplay* oled = new OLEDDisplay(w, h, twi, rst_pin);
    add(oled);
    return oled;
}

Segment* PeripheralFactory::createSegment(int dataPin, int clkPin, int csPin, int numDevices) {
    Segment* segment = new Segment(dataPin, clkPin, csPin, numDevices);
    add(segment);
    return segment;
}

Encoder* PeripheralFactory::createEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, int16_t step) {
    Encoder* encoder = new Encoder(pinA, pinB, pinSW, minVal, maxVal, step);
    add(encoder);
    return encoder;
}

RGBLED* PeripheralFactory::createRGBLED(uint8_t pin, uint16_t numPixels, neoPixelType type) {
    RGBLED* rgbled = new RGBLED(pin, numPixels, type);
    add(rgbled);
    return rgbled;
}

Buzzer* PeripheralFactory::createBuzzer(uint8_t pin) {
    Buzzer* buzzer = new Buzzer(pin);
    add(buzzer);
    return buzzer;
}

void PeripheralFactory::init() {
    for (int i = 0; i < _peripheralCount; i++) {
        if (_peripherals[i] != nullptr) {
            _peripherals[i]->init();
        }
    }
}

void PeripheralFactory::update() {
    for (int i = 0; i < _peripheralCount; i++) {
        if (_peripherals[i] != nullptr) {
            _peripherals[i]->update();
        }
    }
}