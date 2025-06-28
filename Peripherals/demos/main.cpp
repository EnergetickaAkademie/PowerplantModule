#include <Arduino.h>
#include "wemos_pins.h"
#include "PeripheralFactory.h"
#include "ota.h"
#include "secrets.h"
#include <Wire.h>

PeripheralFactory factory;
//LiquidCrystal* lcd;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup(){
	Serial.begin(115200);
	Wire.begin(); // Initialize I2C bus

	//lcd = factory.createLiquidCrystal(0x27, 20, 4);
	
	//connectWifi(SECRET_SSID, SECRET_PASSWORD);
	//setupOTA();

	factory.init();

	// if (lcd){
	// 	lcd->setCursor(0, 0);
	// 	lcd->print("Hello, World!");
	// 	lcd->setCursor(0, 1);
	// 	lcd->print("LCD Ready!");
	// }
	
	lcd.init();
	lcd.backlight();

	Serial.println(F("Ready"));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());
}


void loop() {
	//handleOTA();
	factory.update();

	// Update LCD every second
	// if ((millis() % 100) == 0) {
	// 	if (lcd) {
	// 		lcd->setCursor(0, 2);
	// 		lcd->print("Time: ");
	// 		lcd->print(millis() / 1000);
	// 		lcd->print("s");
	// 		lcd->setCursor(0, 3);
	// 		lcd->print("IP: ");
	// 		lcd->print(WiFi.localIP().toString().c_str());

	// 	}
	// }

	lcd.setCursor(0,0);
	lcd.print("HTestset");

	lcd.setCursor(0,1);

	lcd.print("Aaasdasdasd");

}