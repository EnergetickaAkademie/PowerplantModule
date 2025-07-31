#include "PeripheralFactory.h"
#include <Arduino.h>

PeripheralFactory::PeripheralFactory() {

}

PeripheralFactory::~PeripheralFactory() {
	for (auto& peripheral : _peripherals) {
		delete peripheral;
		peripheral = nullptr;
	}
}

void PeripheralFactory::add(Peripheral* peripheral) {
	if (peripheral) {
		_peripherals.push_back(peripheral);
	} else {
		Serial.println(F("Error: Attempted to add a null peripheral."));
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

Encoder* PeripheralFactory::createEncoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, uint16_t speedup_ms, uint8_t speedup_multiplier) {
	Encoder* encoder = new Encoder(pinA, pinB, pinSW, minVal, maxVal, speedup_ms, speedup_multiplier);
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

ShiftRegisterChain* PeripheralFactory::createShiftRegisterChain(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin) {
	ShiftRegisterChain* chain = new ShiftRegisterChain(latchPin, dataPin, clockPin);
	add(chain);
	return chain;
}

Button* PeripheralFactory::createButton(uint8_t pin) {
	Button* button = new Button(pin);
	add(button);
	return button;
}

LEDButton* PeripheralFactory::createLEDButton(uint8_t buttonPin, uint8_t ledPin) {
	LEDButton* ledButton = new LEDButton(buttonPin, ledPin);
	add(ledButton);
	return ledButton;
}

Periodic* PeripheralFactory::createPeriodic(unsigned long interval, std::function<void()> callback) {
	Periodic* periodic = new Periodic(interval, callback);
	add(periodic);
	return periodic;
}

// --- Shift Register Device Factory Methods ---

Bargraph* PeripheralFactory::createBargraph(ShiftRegisterChain* chain, uint8_t numLeds) {
	if (!chain) return nullptr;

	Bargraph* bargraph = new Bargraph(numLeds);
	chain->addDevice(bargraph);
	return bargraph;
}

SegmentDisplay* PeripheralFactory::createSegmentDisplay(ShiftRegisterChain* chain, uint8_t numDigits) {
	if (!chain) return nullptr;

	SegmentDisplay* display = new SegmentDisplay(numDigits);
	chain->addDevice(display);
	return display;
}

LiquidCrystal* PeripheralFactory::createLiquidCrystal(uint8_t address, uint8_t cols, uint8_t rows) {
	LiquidCrystal* lcd = new LiquidCrystal(address, cols, rows);
	add(lcd);
	return lcd;
}

void PeripheralFactory::update() {
	for (auto& peripheral : _peripherals) {
		if (peripheral) {
			peripheral->update();
		}
	}
}