#include "periodic.h"
#include <Arduino.h>

Periodic::Periodic(unsigned long interval, std::function<void()> callback) {
	this->interval = interval;
	this->callback = callback;
	this->lastRun = 0;
}

void Periodic::setInterval(unsigned long interval) {
	this->interval = interval;
}

void Periodic::setCallback(std::function<void()> callback) {
	this->callback = callback;
}

void Periodic::update() {
	if (millis() - lastRun >= interval) {
		lastRun = millis();
		if (callback) {
			callback();
		}
	}
}
