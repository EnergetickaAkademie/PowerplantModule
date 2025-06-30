# OneWire Host Example for ESP32-S3-DEV-KIT-N8R8

This project is a OneWire host example specifically configured for the ESP32-S3-DEV-KIT-N8R8 development board.

## Hardware Requirements

- **ESP32-S3-DEV-KIT-N8R8** (ESP32-S3 with 8MB Flash, 8MB PSRAM)
- OneWire slaves (compatible with the com-prot library)
- USB-C cable for programming and power

## Board Specifications

- **MCU**: ESP32-S3 (Dual-core Xtensa LX7)
- **Flash**: 8MB
- **PSRAM**: 8MB QSPI
- **USB**: Native USB support (CDC)
- **WiFi**: 802.11 b/g/n
- **Bluetooth**: Bluetooth 5.0 LE

## Pin Configuration

- **OneWire Communication**: GPIO 18
- **USB Serial**: Native USB CDC (no external USB-to-Serial chip needed)

## Key Features

- **PJON-based OneWire Communication**: Uses the com-prot library for robust communication
- **OTA Updates**: Over-the-air firmware updates
- **WebSerial Interface**: Web-based serial monitor accessible via browser
- **PSRAM Support**: Utilizes the 8MB PSRAM for enhanced performance
- **Native USB**: Direct USB communication without external converter

## Configuration Details

### PlatformIO Configuration

The `platformio.ini` file is configured with:

- **Board**: `esp32-s3-devkitc-1` (compatible with ESP32-S3-DEV-KIT-N8R8)
- **Memory Type**: `qio_opi` for optimal PSRAM performance
- **USB Mode**: Native USB with CDC enabled
- **Build Flags**:
  - `BOARD_HAS_PSRAM`: Enables PSRAM support
  - `ARDUINO_USB_MODE=1`: Enables native USB
  - `ARDUINO_USB_CDC_ON_BOOT=1`: Enables USB CDC on boot

### Libraries Used

- **PJON**: Low-level communication protocol
- **com-prot**: High-level communication protocol wrapper
- **ota**: Over-the-air update functionality
- **WebSerial**: Web-based serial interface
- **ESPAsyncWebServer**: Async web server for WebSerial

## Setup Instructions

1. **Install PlatformIO**: Make sure you have PlatformIO installed in your IDE
2. **Clone/Copy Project**: Copy this project to your workspace
3. **Configure WiFi**: Edit `include/secrets.h` with your WiFi credentials
4. **Build Project**: Use PlatformIO to build the project
5. **Upload Firmware**: Connect the ESP32-S3 via USB-C and upload

## Usage

### Initial Setup

1. Connect the ESP32-S3-DEV-KIT-N8R8 to your computer via USB-C
2. The board will appear as a serial device
3. Upload the firmware using PlatformIO
4. Open the serial monitor to see the startup messages

### OneWire Communication

- The host will automatically scan for OneWire slaves
- Connected slaves will appear in the serial output and WebSerial interface
- The example sends periodic commands to demonstrate communication

### Web Interface

Once connected to WiFi, you can access:

- **WebSerial**: `http://<ESP32_IP>/webserial` - Web-based serial monitor
- **OTA Updates**: Upload new firmware via the web interface

### Example Commands

The host automatically sends these example commands:

- **LED Toggle (0x10)**: Toggles LEDs on type 1 slaves every 5 seconds
- **Temperature Request (0x20)**: Requests temperature from type 2 slaves

## Monitoring

### Serial Output

The serial monitor will show:
- Boot messages and system information
- WiFi connection status
- Connected slave information
- Communication debug messages
- Memory usage statistics

### WebSerial

Access the web interface for:
- Real-time communication monitoring
- Remote debugging without USB connection
- Clean, browser-based interface

## Troubleshooting

### USB Connection Issues

If the board doesn't appear as a serial device:
1. Ensure you're using a data-capable USB-C cable
2. Try pressing the BOOT button while connecting
3. Check that the board is getting power (LED should be on)

### WiFi Connection Issues

1. Verify WiFi credentials in `secrets.h`
2. Check that your WiFi network is 2.4GHz (ESP32 doesn't support 5GHz)
3. Monitor serial output for connection status

### OneWire Communication Issues

1. Verify that GPIO 18 is properly connected to the OneWire bus
2. Check that slaves are powered and configured correctly
3. Monitor debug output for communication errors

## Advanced Configuration

### Changing OneWire Pin

To use a different GPIO pin for OneWire communication:
1. Modify the pin number in `main.cpp`: `ComProtMaster master(1, NEW_PIN);`
2. Update any hardware connections accordingly

### Memory Optimization

The ESP32-S3 with 8MB PSRAM provides ample memory. Monitor usage with:
- `ESP.getFreeHeap()`: Available heap memory
- `ESP.getMinFreeHeap()`: Minimum free heap during runtime
- `ESP.getPsramSize()`: Total PSRAM size

### Custom Commands

Add your own commands by:
1. Defining new message types in the com-prot protocol
2. Implementing handlers in the `debugReceiveHandler` function
3. Adding transmission logic in the main loop

## Development Notes

- The ESP32-S3 supports dual-core processing for advanced applications
- Native USB eliminates the need for external USB-to-Serial converters
- PSRAM allows for larger data buffers and more complex applications
- Built-in WiFi and Bluetooth provide multiple connectivity options

## License

This project follows the same license as the parent PowerplantModule project.
