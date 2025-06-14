#include <Arduino.h>

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
	//blink the LED every second
	digitalWrite(LED_BUILTIN, HIGH);
	delay(1000);
	digitalWrite(LED_BUILTIN, LOW);
	delay(1000);
}