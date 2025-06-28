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
OLEDDisplay* oled = factory.createOLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);
LED* led = factory.createLed(D4);
Encoder* encoder = factory.createEncoder(ENCODER_PIN_A, ENCODER_PIN_B, ENCODER_PIN_SW, 0, 255, 1);
RGBLED* rgb = factory.createRGBLED(D6);
//Buzzer* buzzer = factory.createBuzzer(D4);

//Motor* motorA = factory.createMotor(MOTOR_A_IA_PIN, MOTOR_A_IB_PIN);

//Segment* segment = factory.createSegment(MAX7219_DATA_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, MAX7219_NUM_DEVICES);

void setup() {
    Serial.begin(115200);
    Serial.println(F("Booting..."));

    setupOTA();

    factory.init();

    Serial.println(F("Motor pins initialized."));

    // segment.begin();
    // segment.setBrightness(5);

    Serial.println(F("Ready"));
    Serial.print(F("IP address: "));
    Serial.println(WiFi.localIP());

    //led->startBlink(500);

    rgb->setColor(255, 0, 0);
    rgb->setBrightness(50);
    rgb->show();

    led->on();
    //rotary.setChangedHandler(rotation_callback);
}

void loop() {
    enum RGBMode { MODE_R, MODE_G, MODE_B, MODE_BRIGHTNESS };
    static RGBMode mode = MODE_R;
    static uint8_t r = 0, g = 0, b = 0, brightness = 255;
    static int16_t lastEncoderValue = -1;

    // Set encoder range for each mode
    auto setEncoderRange = [&]() {
        switch (mode) {
            case MODE_R: encoder->setValue(r); break;
            case MODE_G: encoder->setValue(g); break;
            case MODE_B: encoder->setValue(b); break;
            case MODE_BRIGHTNESS: encoder->setValue(brightness); break;
        }
    };

    factory.update();

    handleOTA();

    unsigned long currentMillis = millis();

    if (encoder->isButtonPressed()) {
        mode = static_cast<RGBMode>((mode + 1) % 4);
        setEncoderRange();
        //buzzer->buzz(100);
    }

    int16_t val = encoder->getValue();
    if (val != lastEncoderValue) {
        lastEncoderValue = val;
        switch (mode) {
            case MODE_R: r = val; led->setBrightness(r); break;
            case MODE_G: g = val; break;
            case MODE_B: b = val; break;
            case MODE_BRIGHTNESS: brightness = val; break;
        }
    }

    if (currentMillis % 50 == 0) {
        oled->clear();
        oled->setCursor(0, 0);
        oled->setTextSize(1);
        oled->setTextColor(SSD1306_WHITE);
        oled->print(F("Mode: "));
        switch (mode) {
            case MODE_R: oled->println(F("R")); break;
            case MODE_G: oled->println(F("G")); break;
            case MODE_B: oled->println(F("B")); break;
            case MODE_BRIGHTNESS: oled->println(F("Brightness")); break;
        }
        oled->print(F("R: ")); oled->println(r);
        oled->print(F("G: ")); oled->println(g);
        oled->print(F("B: ")); oled->println(b);
        oled->print(F("Br: ")); oled->println(brightness);
        oled->show();
    }
    
    rgb->setColor(r, g, b);
    rgb->setBrightness(brightness);
    rgb->show();

    //rotary.loop();

    //Serial.print(F("Encoder Value: "));
    //Serial.println(encoder->getValue());
}