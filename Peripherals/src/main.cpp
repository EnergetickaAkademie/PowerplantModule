#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "secrets.h"
#include <ota.h>

#include "wemos_pins.h"
#include <segment.h>
#include <oled.h>
//#include <rfid_reader.h>

//#ifndef RFID_SS_PIN
//#define RFID_SS_PIN   D8  // ESP8266 GPIO15 (D8 on Wemos D1 Mini) for RFID SS/SDA
//#endif
//#ifndef RFID_RST_PIN
//#define RFID_RST_PIN  D3  // ESP8266 GPIO0 (D3 on Wemos D1 Mini) for RFID RST
//#endif // Commented out or remove definition

#define BLINK_INTERVAL 500
#define DISPLAY_UPDATE_INTERVAL 1000
//#define RFID_CHECK_INTERVAL 250

#define MAX7219_DATA_PIN D1
#define MAX7219_CLK_PIN  D2
#define MAX7219_CS_PIN   D0
#define MAX7219_NUM_DEVICES 3

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET_PIN -1
#define I2C_ADDRESS 0x3C

unsigned long lastDisplayUpdateTime = 0;
int displayNumber = 0;

//Segment segment(MAX7219_DATA_PIN, MAX7219_CLK_PIN, MAX7219_CS_PIN, MAX7219_NUM_DEVICES);
//RFIDReader rfidReader(RFID_SS_PIN); // Updated instantiation, removed RFID_RST_PIN

//unsigned long lastRfidCheckTime = 0;

OLEDDisplay oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET_PIN);

void setup() {
	Serial.begin(115200);
	Serial.println(F("Booting"));
	
	connectWifi(SECRET_SSID, SECRET_PASSWORD);
	
	setupOTA();

	//segment.begin(); // Uncommented
	//segment.setBrightness(5); // Uncommented

	if (!oled.begin(I2C_ADDRESS)) {
		Serial.println(F("SSD1306 allocation failed"));
		for (;;);
	}

	//rfidReader.begin(&Serial); 
	Serial.println(F("RFID Reader Initialized with MFRC522v2 library."));
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

	Serial.println(F("Ready"));
	Serial.print(F("IP address: "));
	Serial.println(WiFi.localIP());

	//pinMode(LED_BUILTIN, OUTPUT);

	oled.show();
	delay(2000);
	oled.clear();
	oled.setTextColor(SSD1306_WHITE);
	oled.setTextSize(1);
}

void blinkLED() {
	digitalWrite(LED_BUILTIN, HIGH);
	delay(BLINK_INTERVAL);
	digitalWrite(LED_BUILTIN, LOW);
	delay(BLINK_INTERVAL);
}

/*void handleRFID() {
	// Simplified approach using direct MFRC522 access like Random Nerd Tutorials
	MFRC522& mfrc522 = rfidReader.getMFRC522Instance();
	
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	Serial.println(F("\n--- Card Detected (Direct MFRC522 Access) ---"));
	Serial.print(F("Card UID: "));
	MFRC522Debug::PrintUID(Serial, mfrc522.uid);
	Serial.println();

	// Get the card type
	MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
	Serial.print(F("PICC type: "));
	Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));	// For Ultralight cards, try to extract the data manually from the dump
	if (piccType == MFRC522::PICC_Type::PICC_TYPE_MIFARE_UL) {
		Serial.println(F("Detected MIFARE Ultralight/NTAG card."));
		
		// IMPORTANT: Try to read data BEFORE calling PICC_DumpToSerial 
		// which might change the card communication state
		Serial.println(F("\n=== EXTRACTING READABLE TEXT ==="));
		
		byte buffer[18]; // Buffer for reading data
		byte bufferSize;
		String extractedText = "";
		bool dataReadSuccessfully = false;
		
		// Read pages 4-7 which typically contain NDEF payload for NTAG cards
		for (byte page = 4; page <= 7; page++) {
			bufferSize = sizeof(buffer);
			MFRC522::StatusCode status = mfrc522.MIFARE_Read(page, buffer, &bufferSize);
			
			if (status == MFRC522::StatusCode::STATUS_OK) {
				Serial.print(F("Page ")); Serial.print(page); Serial.print(F(" data: "));
				for (byte i = 0; i < 4; i++) { // Each page is 4 bytes for Ultralight
					Serial.print(buffer[i], HEX); Serial.print(F(" "));
					// Extract printable characters
					if (buffer[i] >= 0x20 && buffer[i] <= 0x7E) { // Printable ASCII range
						extractedText += (char)buffer[i];
					}
				}
				Serial.println();
				dataReadSuccessfully = true;
			} else {
				Serial.print(F("Failed to read page ")); Serial.print(page);
				Serial.print(F(": ")); Serial.println(MFRC522Debug::GetStatusCodeName(status));
			}
		}
		
		if (dataReadSuccessfully) {
			Serial.print(F("SUCCESS! Extracted readable text: '"));
			Serial.print(extractedText);
			Serial.println(F("'"));
		} else {
			Serial.println(F("All page reads failed before dump."));
		}
		
		Serial.println(F("\nFull card dump (after text extraction):"));
		MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &mfrc522.uid);
		
	}else if (piccType == MFRC522::PICC_Type::PICC_TYPE_MIFARE_1K || 
	           piccType == MFRC522::PICC_Type::PICC_TYPE_MIFARE_4K) {
		Serial.println(F("Detected MIFARE Classic card."));
		Serial.println(F("Full card dump:"));
		MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &mfrc522.uid);
	} else {
		Serial.println(F("Card type not specifically handled, but showing dump:"));
		MFRC522Debug::PICC_DumpToSerial(mfrc522, Serial, &mfrc522.uid);
	}

	// Halt the card (PICC_DumpToSerial should do this automatically, but let's be explicit)
	mfrc522.PICC_HaltA();
	
	delay(1000); // Brief delay before allowing next read
}
*/

void loop() {
	handleOTA();
	//blinkLED();

	unsigned long currentTime = millis();

	// if (currentTime - lastDisplayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
	// 	lastDisplayUpdateTime = currentTime;

	// 	displayNumber++;
	// 	if (displayNumber > 9999) {
	// 		displayNumber = 0;
	// 	}

	// 	segment.displayPower(0, displayNumber);
	// 	segment.displayPower(1, 2 * displayNumber);
	// 	segment.displayPower(2, 4*displayNumber);
	// 	segment.displayPower(3, 8*displayNumber);
	// 	segment.displayPower(4, 16*displayNumber);
	// 	segment.displayPower(5, (float)(32.3*displayNumber));
	// }
}