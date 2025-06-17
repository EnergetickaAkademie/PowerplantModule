#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// MFRC522 pin definitions for Wemos D1 Mini
#define SS_PIN    D8   // SDA pin
#define RST_PIN   D3   // RST pin

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Function declarations
void readNDEFData();
void parseNDEFMessage(byte* data, int length);
String parseTextRecord(byte* data, int length);

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== MFRC522 NFC Forum Type 2 Reader ===");
  Serial.println("Initializing...");
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize MFRC522
  mfrc522.PCD_Init();
  
  // Show details of the MFRC522 reader
  mfrc522.PCD_DumpVersionToSerial();
  
  Serial.println("Ready to read NFC Forum Type 2 tags!");
  Serial.println("Place an NFC tag near the reader...");
  Serial.println();
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  Serial.println("=== NFC Tag Detected ===");
  
  // Show tag UID
  Serial.print("Tag UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  // Show tag type
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("Tag Type: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
  // Check if it's an NTAG or similar NFC Forum Type 2 tag
  if (piccType == MFRC522::PICC_TYPE_MIFARE_UL) {
    Serial.println("NFC Forum Type 2 tag detected (NTAG/MIFARE Ultralight)");
    readNDEFData();
  } else {
    Serial.println("Not an NFC Forum Type 2 tag");
  }
  
  // Halt the card
  mfrc522.PICC_HaltA();
  
  Serial.println("========================");
  Serial.println();
  
  delay(2000);  // Wait 2 seconds before next read
}

void readNDEFData() {
  byte buffer[18];
  byte size = sizeof(buffer);
  MFRC522::StatusCode status;
  
  Serial.println("Reading NDEF data from NFC Forum Type 2 tag...");
  
  // For NTAG213/215/216, NDEF data typically starts at page 4
  // Each page is 4 bytes, and we can read 4 pages at a time (16 bytes total)
  
  // Read capability container (CC) at page 3
  status = mfrc522.MIFARE_Read(3, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Failed to read CC: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  Serial.print("Capability Container: ");
  for (int i = 0; i < 4; i++) {
    Serial.print(buffer[i] < 0x10 ? "0" : "");
    Serial.print(buffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  // Check if NDEF is present (magic number 0xE1)
  if (buffer[0] != 0xE1) {
    Serial.println("No NDEF data found (CC magic number not present)");
    return;
  }
  
  // Read NDEF data starting from page 4
  byte ndefData[512];  // Buffer for NDEF data
  int ndefLength = 0;
  
  // Read multiple pages to get the full NDEF message
  for (int page = 4; page < 40; page += 4) {  // Read up to page 40
    status = mfrc522.MIFARE_Read(page, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
      Serial.print("Failed to read page ");
      Serial.print(page);
      Serial.print(": ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      break;
    }
    
    // Copy data to NDEF buffer
    for (int i = 0; i < 16 && (ndefLength + i) < sizeof(ndefData); i++) {
      ndefData[ndefLength + i] = buffer[i];
    }
    ndefLength += 16;
    
    // If we encounter terminator TLV (0xFE), stop reading
    for (int i = 0; i < 16; i++) {
      if (buffer[i] == 0xFE) {
        Serial.println("Found NDEF terminator");
        goto parse_ndef;
      }
    }
  }
  
parse_ndef:
  Serial.print("Read ");
  Serial.print(ndefLength);
  Serial.println(" bytes of data");
  
  // Print raw data for debugging
  Serial.println("Raw NDEF data:");
  for (int i = 0; i < min(ndefLength, 64); i++) {  // Print first 64 bytes
    if (i % 16 == 0) {
      Serial.printf("\n%04X: ", i);
    }
    Serial.printf("%02X ", ndefData[i]);
  }
  Serial.println();
  
  // Parse NDEF message
  parseNDEFMessage(ndefData, ndefLength);
}

void parseNDEFMessage(byte* data, int length) {
  Serial.println("\n=== Parsing NDEF Message ===");
  
  if (length < 3) {
    Serial.println("Data too short for NDEF message");
    return;
  }
  
  int pos = 0;
  
  // Look for NDEF TLV (Type-Length-Value)
  // NDEF TLV starts with 0x03
  while (pos < length - 1) {
    if (data[pos] == 0x03) {  // NDEF Message TLV
      Serial.println("Found NDEF Message TLV");
      pos++;  // Move past the type byte
      
      // Get length
      int ndefLength = data[pos];
      pos++;
      
      Serial.print("NDEF Message length: ");
      Serial.println(ndefLength);
      
      if (pos + ndefLength > length) {
        Serial.println("NDEF length exceeds available data");
        return;
      }
      
      // Parse NDEF records
      int recordPos = 0;
      while (recordPos < ndefLength) {
        byte flags = data[pos + recordPos];
        recordPos++;
        
        Serial.print("Record flags: 0x");
        Serial.println(flags, HEX);
        
        // Check if this is the first record and if it's a Text record
        if ((flags & 0x01) && (flags & 0x10)) {  // MB (first record) and SR (short record)
          byte typeLength = data[pos + recordPos];
          recordPos++;
          
          byte payloadLength = data[pos + recordPos];
          recordPos++;
          
          Serial.print("Type length: ");
          Serial.println(typeLength);
          Serial.print("Payload length: ");
          Serial.println(payloadLength);
          
          // Read type
          String recordType = "";
          for (int i = 0; i < typeLength; i++) {
            recordType += (char)data[pos + recordPos + i];
          }
          recordPos += typeLength;
          
          Serial.print("Record type: ");
          Serial.println(recordType);
          
          // If it's a Text record, parse the text
          if (recordType == "T") {
            String text = parseTextRecord(data + pos + recordPos, payloadLength);
            Serial.print("Text content: \"");
            Serial.print(text);
            Serial.println("\"");
          } else {
            Serial.println("Non-text record type");
          }
          
          recordPos += payloadLength;
        } else {
          Serial.println("Unsupported record format");
          break;
        }
      }
      
      return;  // Found and processed NDEF message
    }
    pos++;
  }
  
  Serial.println("No NDEF Message TLV found");
}

String parseTextRecord(byte* data, int length) {
  if (length < 3) {
    return "Invalid text record";
  }
  
  byte statusByte = data[0];
  byte langCodeLength = statusByte & 0x3F;  // Lower 6 bits
  
  Serial.print("Language code length: ");
  Serial.println(langCodeLength);
  
  // Skip language code
  int textStart = 1 + langCodeLength;
  
  if (textStart >= length) {
    return "Invalid text record structure";
  }
  
  // Extract text
  String text = "";
  for (int i = textStart; i < length; i++) {
    text += (char)data[i];
  }
  
  return text;
}
