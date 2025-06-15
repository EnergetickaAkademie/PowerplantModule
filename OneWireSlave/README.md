# PJON ESP8266 Temperature Sensor Slave

A reliable temperature sensor slave using the PJON protocol for robust communication with master devices. PJON provides better error handling, timing tolerance, and collision detection compared to OneWire.

## 🎯 **Core Functionality**

- **Temperature Simulation**: Oscillates between 20°C and 25°C
- **PJON Slave**: Responds to temperature requests from master
- **Heartbeat Response**: Acknowledges master heartbeat signals
- **WebSerial Debug**: Remote monitoring via web interface
- **OTA Updates**: Wireless firmware updates

## 🚀 **Quick Start**

### 1. Hardware Setup
- Connect ESP8266 GPIO2 (D1) to PJON communication bus
- Add 4.7kΩ pull-up resistor between bus and 3.3V (one per bus)
- Connect common ground between all devices

### 2. Configuration
Edit `src/main.cpp` to set unique slave ID:
```cpp
#define SLAVE_ID 10  // Change for each slave: 10, 11, 12, etc.
```

### 3. Upload and Test
```bash
pio run -e d1_mini --target upload
pio device monitor
```

## 📋 **PJON Protocol**

### Supported Messages
- **Temperature Request**: Master requests current temperature
- **Heartbeat**: Master sends keepalive signal
- **Status Request**: Master checks device status

### Response Format
Temperature responses include:
- Message type identifier
- 4-byte float temperature value
- Slave ID for identification

## 🔧 **Hardware Requirements**

- **MCU**: ESP8266 (D1 Mini recommended)
- **Communication**: Single wire + ground (GPIO2/D1)
- **Pull-up**: 4.7kΩ resistor (shared across bus)
- **Power**: 3.3V stable supply

## 🌐 **Remote Access**

### WebSerial Interface
- **URL**: `http://PjonSlave.local/webserial`
- **Features**: Real-time monitoring, command interface
- **Data**: Temperature updates, message logs, error reports

### OTA Updates
- **Initial**: USB upload required
- **Subsequent**: Wireless via `pio run -e d1_mini_ota --target upload`
- **Hostname**: `PjonSlave.local`

## 📊 **Expected Output**

```
PJON Temperature Sensor Slave
PJON Slave initialized with ID: 10
Listening on pin: 5

Received message from ID 1, type: 1
Temperature request received
Temperature response sent successfully

Current temperature: 23.2°C

Received message from ID 1, type: 3
Heartbeat received
```

## 🛠️ **vs OneWire Advantages**

| Feature | OneWire | PJON |
|---------|---------|------|
| **Error Handling** | Basic CRC | ACK/NAK + Retry |
| **Timing** | Very strict | Flexible tolerance |
| **Collisions** | Not handled | Automatic detection |
| **Multi-Master** | No | Yes |
| **Reliability** | Good | Excellent |

## 🔧 **Troubleshooting**

### No Communication
1. Check GPIO2 (D1) connections
2. Verify pull-up resistor is present
3. Ensure unique slave IDs
4. Check power supply stability

### WebSerial Issues
1. Verify WiFi connection
2. Try IP address if `.local` fails
3. Check firewall settings

## 📚 **Development**

### Adding Custom Commands
Extend the message handler in `receiver_function()`:

```cpp
case CUSTOM_COMMAND:
    // Handle custom command
    break;
```

### Changing Update Rate
Modify the blinking interval:

```cpp
constexpr uint32_t interval = 2000; // milliseconds
```

## 📖 **Related Documentation**

- `../PJON_SETUP_GUIDE.md` - Complete setup guide
- `WEBSERIAL_GUIDE.md` - WebSerial usage
- `OTA_SETUP.md` - Over-the-air updates

---

**Ready to deploy!** This slave device provides reliable temperature data via the robust PJON protocol.
