#include <Arduino.h>
#include "wemos_pins.h"
#include "secrets.h"
#include <ota.h>

// Pin definitions
#define LATCH_PIN D1 // RCLK (latch)
#define DATA_PIN  D2 // SER (data)
#define CLOCK_PIN D0 // SRCLK (clock)

// 7-segment segment encoding for common cathode (a-g,dp)
const byte digitToSegment[] = {
	// pgfedcba
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111,  // 9
	0b10000000  // DP
};

// Digit select (active LOW for common cathode, one-hot)
const byte digitSelect[] = {
	(byte)~(1 << 7),
	(byte)~(1 << 6),
	(byte)~(1 << 5),
	(byte)~(1 << 4),
	(byte)~(1 << 3),
	(byte)~(1 << 2),
	(byte)~(1 << 1),
	(byte)~(1 << 0)
};

void setup() {
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DATA_PIN, OUTPUT);

	//connectWifi(SECRET_SSID, SECRET_PASSWORD);
	//setupOTA();

	Serial.println(F("Ready"));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());
}

void displayNumber(int number) {
	// Break number into digits
	byte digits[4];
	for (int i = 3; i >= 0; i--) {
		digits[i] = number % 10;
		number /= 10;
	}

	// Multiplex each digit
	for (int pos = 0; pos < 4; pos++) {
		digitalWrite(LATCH_PIN, LOW);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitSelect[pos]);
		shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitToSegment[digits[3 - pos]]);
		digitalWrite(LATCH_PIN, HIGH);
		delay(2);
	}
}

void updateDisplayRaw(byte digitSel, byte segmentData) {
	digitalWrite(LATCH_PIN, LOW);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitSel);

	shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, segmentData);

	digitalWrite(LATCH_PIN, HIGH);
}

void loop() {
	handleOTA();

	// displayNumber(1111);
	
	//shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0b00001110);
	//shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0b01001111);

	for (int digitPos = 0; digitPos < 8; digitPos++) {
		for (int num = 0; num <= 10; num++) {
			// Only light up one digit at a time
			digitalWrite(LATCH_PIN, LOW);
			shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitSelect[digitPos]);
			shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, digitToSegment[num]);
			digitalWrite(LATCH_PIN, HIGH);
			delay(200); // Hold the digit for 400ms
		}
	}

	// updateDisplayRaw(0b11111110, 0b11111111);

	// delay(1000);

	// updateDisplayRaw(0b11111101, 0b11111111);

	// delay(1000);

	// updateDisplayRaw(0b11111011, 0b11111111);

	// delay(1000);

	// updateDisplayRaw(0b11110111, 0b11111111);

	// delay(1000);
}