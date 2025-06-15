#include "rfid_reader.h"

// Constructor
RFIDReader::RFIDReader(byte ssPin) 
    : _ss_pin_driver(ssPin),      // Initialize the SS pin driver
      _spi_driver(_ss_pin_driver),  // Initialize SPI driver with the SS pin driver
      _mfrc522(_spi_driver),      // Initialize MFRC522 with the SPI driver
      _cardReadSuccessfully(false),
      _detectedType(CardType::UNKNOWN) {
    // _rstPin related logic removed
}

void RFIDReader::begin(Stream* debugStream) {
    _mfrc522.PCD_Init();
    if (debugStream) {
        MFRC522Debug::PCD_DumpVersionToSerial(_mfrc522, *debugStream);
    }
}

bool RFIDReader::isCardPresent() {
    return _mfrc522.PICC_IsNewCardPresent();
}

bool RFIDReader::readCardSerial() {
    // PICC_ReadCardSerial() attempts to select a card and read its UID.
    // It stores the UID in _mfrc522.uid.
    _cardReadSuccessfully = _mfrc522.PICC_ReadCardSerial();
    if (_cardReadSuccessfully) {
        // Detect and store card type
        MFRC522::PICC_Type piccType = _mfrc522.PICC_GetType(_mfrc522.uid.sak);
        switch (piccType) {
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_MINI:
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_1K:
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_4K:
                _detectedType = CardType::MIFARE_CLASSIC_1K; // Grouping classic types
                break;
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_UL:
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_PLUS: // Plus can sometimes act like UL or Classic
            case MFRC522::PICC_Type::PICC_TYPE_MIFARE_DESFIRE: // More complex, but for simplicity
                _detectedType = CardType::MIFARE_ULTRALIGHT; // Grouping UL and some others that don't use classic auth for basic read
                break;
            case MFRC522::PICC_Type::PICC_TYPE_ISO_14443_4:
            case MFRC522::PICC_Type::PICC_TYPE_ISO_18092:
            case MFRC522::PICC_Type::PICC_TYPE_NOT_COMPLETE:
            default:
                _detectedType = CardType::UNKNOWN;
                break;
        }
    } else {
        _detectedType = CardType::UNKNOWN;
    }
    return _cardReadSuccessfully;
}

RFIDReader::CardType RFIDReader::getDetectedCardType() {
    if (!_cardReadSuccessfully) return CardType::UNKNOWN;
    return _detectedType;
}

String RFIDReader::getUID() {
    if (!_cardReadSuccessfully) {
        return ""; 
    }
    String uidStr = "";
    for (byte i = 0; i < _mfrc522.uid.size; i++) {
        if (_mfrc522.uid.uidByte[i] < 0x10) {
            uidStr += "0"; // Add leading zero for single-digit hex
        }
        uidStr += String(_mfrc522.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();
    return uidStr;
}

void RFIDReader::printCardDetailsToSerial(Stream& serialStream) {
    if (_cardReadSuccessfully) {
        // MFRC522Debug::PICC_DumpToSerial handles printing various details.
        // It requires the MFRC522 object, the stream, and a pointer to the UID.
        MFRC522Debug::PICC_DumpToSerial(_mfrc522, serialStream, &(_mfrc522.uid));
    } else {
        serialStream.println(F("No card read successfully to print details."));
    }
}

void RFIDReader::haltCard() {
    _mfrc522.PICC_HaltA();      // Halt PICC
    _mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
}

MFRC522& RFIDReader::getMFRC522Instance() {
    return _mfrc522;
}

// New method to read a data block and return as String
String RFIDReader::readDataBlockAsString(byte blockAddr, MFRC522::MIFARE_Key* key) {
    if (!_cardReadSuccessfully) {
        return F("[Error: No card selected]");
    }
    if (_detectedType != CardType::MIFARE_CLASSIC_1K) {
        return F("[Error: Not a MIFARE Classic card]");
    }

    String result = "";
    byte buffer[18]; // Buffer to read 16 bytes + 2 for CRC
    byte size = sizeof(buffer);
    MFRC522::StatusCode status;

    // 1. Authenticate the sector for the given block
    // Serial.print(F("Authenticating block ")); Serial.print(blockAddr); Serial.println(F("..."));
    status = _mfrc522.PCD_Authenticate(MFRC522::PICC_Command::PICC_CMD_MF_AUTH_KEY_A, blockAddr, key, &(_mfrc522.uid));
    if (status != MFRC522::StatusCode::STATUS_OK) {
        // Serial.print(F("PCD_Authenticate() failed: "));
        // Serial.println(MFRC522Debug::GetStatusCodeName(status));
        return F("[Auth Error]");
    }
    // Serial.println(F("Authentication successful."));

    // 2. Read the block
    // Serial.print(F("Reading data from block ")); Serial.print(blockAddr); Serial.println(F("..."));
    status = _mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::StatusCode::STATUS_OK) {
        // Serial.print(F("MIFARE_Read() failed: "));
        // Serial.println(MFRC522Debug::GetStatusCodeName(status));
        return F("[Read Error]");
    }
    // Serial.println(F("Read successful."));

    // 3. Convert buffer to String (first 16 bytes)
    // Stop at the first null character if found, or use all 16 bytes.
    for (byte i = 0; i < 16; i++) {
        if (buffer[i] == 0x00) { // Null terminator
            break;
        }
        if (isprint(buffer[i])) { // Check if it's a printable character
             result += (char)buffer[i];
        } else {
            // Optional: represent non-printable characters, or ignore
            // result += '[' + String(buffer[i], HEX) + ']'; 
        }
    }
    _mfrc522.PCD_StopCrypto1(); 
    return result;
}

// For MIFARE Ultralight (reads a 4-byte page)
String RFIDReader::readUltralightPageAsString(byte pageAddr) {
    if (!_cardReadSuccessfully) {
        return F("[Error: No card selected]");
    }

    String result = "";
    byte buffer[16]; // Max buffer size for Ultralight read response
    byte sendData[2];
    byte sendLen = 2;
    byte backLen = sizeof(buffer); 
    MFRC522::StatusCode status;

    sendData[0] = 0x30; // MIFARE Ultralight Read command
    sendData[1] = pageAddr;

    // Serial.print(F("Attempting to read Ultralight page using PCD_TransceiveData with command 0x30: ")); Serial.println(pageAddr);

    status = _mfrc522.PCD_TransceiveData(sendData, sendLen, buffer, &backLen, nullptr, 0, false);

    if (status != MFRC522::StatusCode::STATUS_OK) {
        // Serial.print(F("PCD_TransceiveData() for UL_READ (0x30) failed: "));
        // Serial.println(MFRC522Debug::GetStatusCodeName(status));
        String errorMsg = F("[UL Read TX Error: ");
        errorMsg += MFRC522Debug::GetStatusCodeName(status);
        errorMsg += F("]");
        return errorMsg;
    }

    if (backLen < 4) {
        // Serial.print(F("PCD_TransceiveData() for UL_READ (0x30) returned less than 4 bytes: ")); Serial.println(backLen);
        return F("[UL Read Error: Not enough data]");
    }

    for (byte i = 0; i < 4; i++) { 
        if (isprint(buffer[i])) {
            result += (char)buffer[i];
        } else {
            // result += "."; 
        }
    }
    return result;
}
