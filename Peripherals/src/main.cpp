#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include <ota.h>

#include "wemos_pins.h" 
#include <segment.h>
#include <oled.h>
#include <motor.h>

// Motor Pin Definitions
const int MOTOR_A_IA_PIN = D5; // GPIO14 for L9110 A-IA
const int MOTOR_A_IB_PIN = D6; // GPIO12 for L9110 A-IB
int motorSpeed = 150;       // Max speed for ESP8266 PWM (0-1023)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET_PIN -1
#define I2C_ADDRESS 0x3C

// MAX7219 Seven Segment Display Definitions (if used)
// #define MAX7219_DATA_PIN D1
// #define MAX7219_CLK_PIN  D2
// #define MAX7219_CS_PIN   D0
// #define MAX7219_NUM_DEVICES 3

// Global Objects
// Motor motorA(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN); // Not using the library object here
OLEDDisplay oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
// Segment segment(MAX7219_DATA_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, MAX7219_NUM_DEVICES);

Motor motor(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting..."));

    setupOTA();

    motor.begin();

    Serial.println(F("Motor pins initialized."));

    if (!oled.begin(I2C_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
    } else {
        oled.show();
        delay(1000);
        oled.clear();
        oled.setTextColor(SSD1306_WHITE);
        oled.setTextSize(1);
        oled.setCursor(0,0);
        oled.print(F("Motor Control Ready"));
        oled.show();
        Serial.println(F("OLED Initialized."));
    }

    // segment.begin();
    // segment.setBrightness(5);

    Serial.println(F("Ready"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());
}

void loop() {
    handleOTA();

    Serial.println(F("Running motor A forward..."));
    oled.clear();
    oled.setCursor(0,10);
    oled.print(F("Motor A: Forward"));
    oled.show();
    motor.forward(motorSpeed);
    delay(3000); 

    Serial.println(F("Stopping motor A..."));
    oled.clear();
    oled.setCursor(0,10);
    oled.print(F("Motor A: Stop"));
    oled.show();
    motor.stop();
    delay(2000); 

    Serial.println(F("Running motor A backward..."));
    oled.clear();
    oled.setCursor(0,10);
    oled.print(F("Motor A: Backward"));
    oled.show();
    motor.forward(motorSpeed);
    delay(3000); 

    Serial.println(F("Stopping motor A..."));
    oled.clear();
    oled.setCursor(0,10);
    oled.print(F("Motor A: Stop"));
    oled.show();
    motor.stop();
    delay(2000); 

    Serial.println(F("Repeating motor test sequence..."));
    oled.clear();
    oled.setCursor(0,10);
    oled.print(F("Sequence Repeat..."));
    oled.show();
}