#pragma once

#include "Peripheral.h"
#include <functional>

class Periodic : public Peripheral {
	private:
		unsigned long interval;
		unsigned long lastRun;
		std::function<void()> callback;

	public:
		Periodic(unsigned long interval, std::function<void()> callback);
		void setInterval(unsigned long interval);
		void setCallback(std::function<void()> callback);
		void update() override;
};
