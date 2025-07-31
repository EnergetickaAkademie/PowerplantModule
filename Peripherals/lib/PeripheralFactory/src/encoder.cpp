#include "encoder.h"
#include <Arduino.h>

// Static variables for ISR handling
static Encoder* encoderInstances[10] = {nullptr};  // Support up to 10 encoders
static uint8_t instanceCount = 0;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint8_t pinSW, int16_t minVal, int16_t maxVal, int16_t steps_per_click,
	bool enable_speedup, unsigned int speedup_increment, unsigned int speedup_interval) : 
	_minVal(minVal), 
	_maxVal(maxVal),
	_stepsPerClick(steps_per_click),
	_pinA(pinA),
	_pinB(pinB),
	_pinSW(pinSW)
{
	// Create instance-specific rotary object
	_rotary = new AiEsp32RotaryEncoder(pinA, pinB, pinSW, -1, steps_per_click, false);
	
	// Initialize before setting up interrupts
	_rotary->begin();
	_rotary->setBoundaries(minVal, maxVal, false);
	_rotary->setAcceleration(enable_speedup ? speedup_increment * 100 : 0);
	
	// Register instance for ISR handling
	if (instanceCount < 10) {
		encoderInstances[instanceCount] = this;
		instanceCount++;
		
		// Set up ISR for this instance's pins
		attachInterrupt(digitalPinToInterrupt(pinA), []{ Encoder::readEncoderISR(); }, CHANGE);
		attachInterrupt(digitalPinToInterrupt(pinB), []{ Encoder::readEncoderISR(); }, CHANGE);
		if (pinSW != 255) {
			attachInterrupt(digitalPinToInterrupt(pinSW), []{ Encoder::readEncoderISR(); }, CHANGE);
		}
	}

	_rotary->setEncoderValue(minVal - 1); //HACK:for some reason sets the value of +1 than the argument???
}

Encoder::~Encoder() {
	// Remove interrupt handlers
	detachInterrupt(digitalPinToInterrupt(_pinA));
	detachInterrupt(digitalPinToInterrupt(_pinB));
	if (_pinSW != 255) {
		detachInterrupt(digitalPinToInterrupt(_pinSW));
	}
	
	// Clean up instance
	delete _rotary;
	
	// Remove from instances array
	for (int i = 0; i < instanceCount; i++) {
		if (encoderInstances[i] == this) {
			encoderInstances[i] = nullptr;
			break;
		}
	}
}

void Encoder::update() {
	// No periodic update needed - handled by interrupts
}

int16_t Encoder::getValue() {
	return _rotary->readEncoder();
}

void Encoder::setValue(int16_t value) {
	_rotary->setEncoderValue(value);
}

bool Encoder::isButtonPressed() {
	return _rotary->isEncoderButtonDown();
}

void Encoder::setRange(int16_t minVal, int16_t maxVal) {
	_minVal = minVal;
	_maxVal = maxVal;
	_rotary->setBoundaries(minVal, maxVal, false);
}

int16_t Encoder::getUpperBound() {
	return _maxVal;
}

int16_t Encoder::getLowerBound() {
	return _minVal;
}

int16_t Encoder::getStepsPerClick() {
	return _stepsPerClick;
}

void Encoder::enable() {
	_rotary->enable();
}

void Encoder::disable() {
	_rotary->disable();
}

void IRAM_ATTR Encoder::readEncoderISR() {
	uint32_t status = GPIO_REG_READ(GPIO_STATUS_REG);
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_REG, status);  // Clear all interrupt flags
	
	for (uint8_t i = 0; i < instanceCount; i++) {
		if (encoderInstances[i] && encoderInstances[i]->_rotary) {
			bool pinAChanged = status & (1 << encoderInstances[i]->_pinA);
			bool pinBChanged = status & (1 << encoderInstances[i]->_pinB);
			
			// Only process rotation if exactly one pin changed
			if ((pinAChanged || pinBChanged) && !(pinAChanged && pinBChanged)) {
				encoderInstances[i]->_rotary->readEncoder_ISR();
			}
			
			// Handle button press separately
			if (encoderInstances[i]->_pinSW != 255 && (status & (1 << encoderInstances[i]->_pinSW))) {
				encoderInstances[i]->_rotary->readEncoder_ISR();
			}
		}
	}
}