#ifndef RFID_READER_H
#define RFID_READER_H

#include <Arduino.h> // For String, byte, Stream
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h> // For SS pin
#include <MFRC522Debug.h>          // For debug functions

class RFIDReader {
public:
    enum class CardType {
        UNKNOWN,
        MIFARE_CLASSIC_1K, // Or more general MIFARE_CLASSIC
        MIFARE_ULTRALIGHT,
        // Add other types as needed
    };

    // Constructor now takes SS pin only.
    RFIDReader(byte ssPin); // rstPin is removed

    void begin(Stream* debugStream = nullptr); // Pass optional Stream for PCD_DumpVersionToSerial
    bool isCardPresent();
    bool readCardSerial(); // Reads card data into library's internal buffer
    String getUID();       // Returns UID as a hex string
    
    // Dumps card info to the provided Stream.
    void printCardDetailsToSerial(Stream& serialStream); 
    
    void haltCard(); // Explicitly halt the card

    // Advanced: Get direct access to the MFRC522 object
    MFRC522& getMFRC522Instance();
    CardType getDetectedCardType(); // New method to get card type

    // For MIFARE Classic
    String readDataBlockAsString(byte blockAddr, MFRC522::MIFARE_Key* key);
    // For MIFARE Ultralight (reads a 4-byte page)
    String readUltralightPageAsString(byte pageAddr);


private:
    MFRC522DriverPinSimple _ss_pin_driver;
    MFRC522DriverSPI _spi_driver;
    MFRC522 _mfrc522;
    
    bool _cardReadSuccessfully; // Flag to track if PICC_ReadCardSerial was successful
    CardType _detectedType; // Store detected card type
};

#endif // RFID_READER_H
