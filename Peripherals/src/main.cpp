#include <Arduino.h>
#include "PeripheralFactory.h"

#define MOTOR_A1 D6
#define MOTOR_A2 D5

PeripheralFactory factory;

Motor* motor = nullptr;


void setup() {
	Serial.begin(115200);

	motor = factory.createMotor(MOTOR_A1, MOTOR_A2, 15);
	motor->enableSpeedup(true);
	motor->setSpeedupConfig(2.5f, 1000);

	factory.createPeriodic(10000, [](){
		motor->forward(50);
	});

	factory.createPeriodic(20005, [](){
		motor->forward(100);
	});
}

void loop() {
	factory.update();
}