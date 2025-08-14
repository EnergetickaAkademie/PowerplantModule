#include <Arduino.h>

// Pin configuration
const int POTENTIOMETER_PIN = 12;

// ADC configuration
const int ADC_RESOLUTION = 12;  // ESP32-S3 has 12-bit ADC
const int ADC_MAX_VALUE = (1 << ADC_RESOLUTION) - 1;  // 4095 for 12-bit

// Voltage divider configuration for logarithmic potentiometer
const float VCC = 3.3;  // ESP32-S3 operating voltage
const float MIN_VOLTAGE = VCC * 0.1;  // 1/10 of VCC (0.33V)
const float MAX_VOLTAGE = VCC * 0.9;  // 9/10 of VCC (2.97V)

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  delay(1000);  // Give time for serial connection to establish
  
  // Configure ADC resolution
  analogReadResolution(ADC_RESOLUTION);
  
  // Set attenuation for the ADC pin to read up to 3.3V
  analogSetAttenuation(ADC_11db);
  
  Serial.println("ESP32-S3 Logarithmic Potentiometer Reader");
  Serial.println("=========================================");
  Serial.println("Pin: 12");
  Serial.println("Range: 0-100 (linearized from logarithmic potentiometer)");
  Serial.println("Min voltage: " + String(MIN_VOLTAGE, 2) + "V");
  Serial.println("Max voltage: " + String(MAX_VOLTAGE, 2) + "V");
  Serial.println("Note: Exponential compensation applied for log pot");
  Serial.println("");
}

void loop() {
  // Read raw ADC value
  int rawValue = analogRead(POTENTIOMETER_PIN);
  
  // Convert to voltage
  float voltage = (rawValue * VCC) / ADC_MAX_VALUE;
  
  // Map voltage to 0-100 range for logarithmic potentiometer
  int mappedValue = 0;
  
  if (voltage <= MIN_VOLTAGE) {
    mappedValue = 0;
  } else if (voltage >= MAX_VOLTAGE) {
    mappedValue = 100;
  } else {
    // First, normalize the voltage to 0.0-1.0 range within our operating window
    float normalizedVoltage = (voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE);
    
    // For logarithmic potentiometer, apply exponential mapping to linearize the output
    // This compensates for the logarithmic taper of the potentiometer
    float linearizedValue = (exp(normalizedVoltage * log(10.0)) - 1.0) / 9.0;
    
    // Clamp to 0.0-1.0 range and convert to 0-100
    linearizedValue = constrain(linearizedValue, 0.0, 1.0);
    mappedValue = (int)(linearizedValue * 100);
  }
  
  // Output results
  Serial.print("Raw ADC: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V | Linearized Value: ");
  Serial.print(mappedValue);
  Serial.println("%");
  
  // Small delay for readability
  delay(100);
}
