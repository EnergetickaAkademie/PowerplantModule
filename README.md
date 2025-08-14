# PowerplantModule - PJON Temperature Sensor Network

A robust temperature sensor network using the PJON protocol for reliable ESP8266 communication. This project provides master-slave temperature monitoring with enhanced stability compared to OneWire implementations.

## 🎯 **Projects Overview**

### OneWireHost (Master)
- **Role**: Temperature data collector and network coordinator
- **Protocol**: PJON with SoftwareBitBang strategy
- **Features**: Slave polling, heartbeat management, data aggregation
- **WebSerial**: `http://PjonMaster.local/webserial`

### OneWireSlave (Slave)
- **Role**: Temperature sensor node
- **Protocol**: PJON responsive slave
- **Features**: Temperature simulation, heartbeat response, status reporting
- **WebSerial**: `http://PjonSlave.local/webserial`

## 🚀 **Key Features**

- **PJON Protocol**: More robust than OneWire with automatic retries
- **Multi-Device Support**: Master can handle multiple slaves
- **Real-time Monitoring**: WebSerial interface for remote debugging
- **OTA Updates**: Wireless firmware updates for both master and slaves
- **Temperature Simulation**: Slaves provide simulated temperature data
- **Heartbeat System**: Master monitors slave health

## 📋 **Quick Setup**

1. **Hardware**: Connect all devices to shared PJON bus (GPIO2/D1)
2. **Master**: Flash OneWireHost to one ESP8266
3. **Slaves**: Flash OneWireSlave to other ESP8266s with unique IDs
4. **Monitor**: Access via WebSerial or USB serial

## 🔧 **Protocol Advantages**

| Feature | OneWire | PJON |
|---------|---------|------|
| Error Handling | Basic CRC | ACK/NAK + Retry |
| Timing Tolerance | Very strict | Flexible |
| Multi-Master | No | Yes |
| Collision Detection | No | Yes |
| Reliability | Good | Excellent |

## 📚 **Documentation**

- `PJON_SETUP_GUIDE.md` - Complete setup and configuration guide
- `OneWireHost/` - Master device implementation
- `OneWireSlave/` - Slave device implementation
- `ArduinoOTA/` - Basic OTA example

## 🛠️ **Development Tools**

- **PlatformIO**: Modern IoT development platform
- **WebSerial**: Browser-based debugging interface
- **OTA**: Wireless firmware updates
- **Git Submodules**: External library management

---

**Converted from OneWire to PJON for enhanced reliability and robustness!**