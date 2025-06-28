#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include <ota.h>

#include "wemos_pins.h" 
#include <PeripheralFactory.h>

// Motor Pin Definitions
//const int MOTOR_A_IA_PIN = D5; // GPIO14 for L9110 A-IA
//const int MOTOR_A_IB_PIN = D6; // GPIO12 for L9110 A-IB
//int motorSpeed = 150;       // Max speed for ESP8266 PWM (0-1023)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET_PIN -1
#define I2C_ADDRESS 0x3C

#define ENCODER_PIN_A  D0 
#define ENCODER_PIN_B  D5 
#define ENCODER_PIN_SW D7

// MAX7219 Seven Segment Display Definitions (if used)
// #define MAX7219_DATA_PIN D1
// #define MAX7219_CLK_PIN  D2
// #define MAX7219_CS_PIN   D0
// #define MAX7219_NUM_DEVICES 3

// Global Objects
// Motor motorA(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN); // Not using the library object here
//OLEDDisplay oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
// Segment segment(MAX7219_DATA_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, MAX7219_NUM_DEVICES);

//Motor motor(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN);

PeripheralFactory factory;
// OLEDDisplay* oled = factory.createOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
// LED* led = factory.createLed(D4);
// Encoder* encoder = factory.createEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_SW, 0, 255, 1);
// RGBLED* rgb = factory.createRGBLED(D6);
//Buzzer* buzzer = factory.createBuzzer(D4);

//Motor* motorA = factory.createMotor(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN);

//Segment* segment = factory.createSegment(MAX7219_DATA_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, MAX7219_NUM_DEVICES);

#define LATCH_PIN D6 // ST_CP on HC595
#define CLOCK_PIN D5 // SH_CP on HC595
#define DATA_PIN  D7 // DS on HC595

void updateShiftRegister(byte data) {
  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);

  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting..."));

    setupOTA();

    factory.init();

    Serial.println(F("Motor pins initialized."));

    Serial.println(F("Ready"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());

    pinMode(LATCH_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
}

void loop() {
    factory.update();

    handleOTA();

    unsigned long currentMillis = millis();

    for (int i = 0; i < 8; i++) {
        // Create a byte with a single bit set at position 'i'
        // This will be 1, 2, 4, 8, 16, 32, 64, 128
        byte dataToSend = 1 << i; 
        updateShiftRegister(dataToSend);
        delay(150); // Wait to see the change
    }

    Serial.println("All off");
    updateShiftRegister(0b00000000); // Send all zeros
    delay(1000);

    // --- Example 3: Turn all LEDs on ---
    Serial.println("All on");
    updateShiftRegister(0b11111111); // Send all ones (255)
    delay(1000);

}