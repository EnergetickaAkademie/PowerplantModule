# MFRC522 NFC/RFID Reader Experiment

This experiment implements NFC/RFID tag reading using the MFRC522 module connected to a Wemos D1 Mini.

## Hardware Setup

### Components Required
- **Wemos D1 Mini** (ESP8266 board)
- **MFRC522 RFID/NFC Reader Module**
- **Jumper wires**
- **NFC tags or RFID cards** (MIFARE Classic compatible)

### Wiring Connections
Connect the MFRC522 to the Wemos D1 Mini according to this table:

| MFRC522 Pin | Wemos D1 Mini Pin | Description |
|-------------|-------------------|-------------|
| RST         | D3                | Reset pin   |
| SDA         | D8                | SPI SS pin  |
| MOSI        | D7                | SPI MOSI    |
| MISO        | D6                | SPI MISO    |
| SCK         | D5                | SPI Clock   |
| 3.3V        | 3V3               | Power       |
| GND         | G                 | Ground      |

## Features
- **UID Reading**: Displays the unique identifier of each NFC tag
- **Tag Type Detection**: Shows the type of NFC/RFID card detected
- **Data Reading**: Attempts to read text data from MIFARE Classic cards
- **Multiple Block Reading**: Reads multiple data blocks from the tag

## Building and Uploading
1. Connect your Wemos D1 Mini via USB
2. Build and upload: `pio run -t upload`
3. Monitor serial output: `pio device monitor`

## Serial Output
The program provides detailed information about detected tags:
```
=== MFRC522 RFID/NFC Reader ===
Initializing...
Firmware Version: 0x88 = (clone)
Ready to read NFC tags!
Place an NFC tag near the reader...

=== NFC Tag Detected ===
Tag UID:  04 A3 B2 C1
Tag Type: MIFARE 1KB
Reading tag contents...
Data in sector 1, block 4:
Raw data:  48 65 6C 6C 6F 20 57 6F 72 6C 64 21 00 00 00 00
Text data: Hello World!
```

## Compatible Tags
- **MIFARE Classic 1K**: Full read/write support
- **MIFARE Classic 4K**: Full read/write support  
- **MIFARE Classic Mini**: Full read/write support
- **Other tag types**: UID reading only

## Use Cases
This NFC reader can be used for:
- **Access control systems**
- **Inventory management**
- **Data logging and retrieval**
- **Smart home automation**
- **Educational RFID projects**

## Notes
- Uses default MIFARE key (0xFF 0xFF 0xFF 0xFF 0xFF 0xFF)
- Reads from sector 1 (blocks 4-7) by default
- For custom data storage, modify the sector and block addresses
- Some commercial NFC tags may have different key requirements
