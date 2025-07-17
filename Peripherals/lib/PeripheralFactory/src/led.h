#ifndef LED_H
#define LED_H

#include "Peripheral.h"
#include <Arduino.h>

/**
 * @brief A simple LED peripheral that can be controlled to turn on, off, blink, or set brightness.
 * 
 * This class provides methods to initialize the LED, control its state, and manage blinking behavior.
 * It uses a PWM-capable pin for brightness control.
 * 
 * `LED led(uint8_t pin);`
 * @param pin The GPIO pin number to which the LED is connected.
 */
class LED : public Peripheral {
public:
	LED(Pin pin);
	LED(LedPin pin) : LED(Pin{pin.val}) {}
	LED(uint8_t pin) : LED(Pin{pin}) {}

	void init() override;
	void update() override;

	void on();
	void off();
	void startBlink(unsigned long interval);
	void stopBlink();
	void setState(bool state);
	void toggle();
	void setBrightness(uint8_t brightness);

private:
	int _pin;
	bool _isBlinking;
	bool _ledState;
	unsigned long _blinkInterval;
	unsigned long _lastToggleTime;
	
	uint8_t _brightness; 
};

#endif // LED_H