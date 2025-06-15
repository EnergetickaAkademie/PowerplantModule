#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ota.h>
#include "secrets.h"

// OneWire setup on pin D1
#define ONE_WIRE_BUS D1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// put function declarations here:
void readTemperature();

void setup() {
  Serial.begin(115200);
  Serial.println("OneWireHost Booting");
  
  // Initialize DS18B20 sensor
  sensors.begin();
  
  // Get number of devices on the bus
  int deviceCount = sensors.getDeviceCount();
  Serial.print("Found ");
  Serial.print(deviceCount);
  Serial.println(" DS18B20 devices on the bus");
  
  // Connect to WiFi and setup OTA
  connectWifi(SECRET_SSID, SECRET_PASSWORD);
  setupOTA(-1, "OneWireHost");
  
  // Setup WebSerial debugging
  setupWebSerial("OneWireHost");
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // put your setup code here, to run once:
}

void loop() {
  // Handle OTA updates
  handleOTA();
  
  // Read temperature every 5 seconds
  static unsigned long lastReading = 0;
  if (millis() - lastReading > 1000) {
    readTemperature();
    lastReading = millis();
  }
  
  // put your main code here, to run repeatedly:
}

void readTemperature() {
  // Request temperature from all devices on the bus
  if(sensors.getDeviceCount() == 0) {
    Serial.println("No DS18B20 devices found on the bus");
    WebSerial.println("No DS18B20 devices found on the bus");
    return;
  }
  sensors.requestTemperatures();
  
  // Get temperature for device index 0 (first device)
  float temperatureC = sensors.getTempCByIndex(0);
  
  // Check if reading was successful
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    String tempMsg = "Temperature: " + String(temperatureC) + "°C";
    Serial.println(tempMsg);
    WebSerial.println(tempMsg);
    
    // Convert to Fahrenheit if needed
    float temperatureF = sensors.toFahrenheit(temperatureC);
    String tempMsgF = "Temperature: " + String(temperatureF) + "°F";
    Serial.println(tempMsgF);
    WebSerial.println(tempMsgF);
  } else {
    Serial.println("Error: Could not read temperature data");
    WebSerial.println("Error: Could not read temperature data");
  }
}
