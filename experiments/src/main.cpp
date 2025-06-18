#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <NFCBuildingRegistry.h>

// MFRC522 pin definitions for Wemos D1 Mini
#define SS_PIN    D8   // SDA pin
#define RST_PIN   D3   // RST pin

// Create MFRC522 instance
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Create building registry
NFCBuildingRegistry buildingRegistry(&mfrc522);

// Forward declarations
void onNewBuilding(uint8_t buildingType, const String& uid);
void onDeleteBuilding(uint8_t buildingType, const String& uid);
void showStatistics();
void showHelp();

void onNewBuilding(uint8_t buildingType, const String& uid) {
  Serial.println("🏢 NEW BUILDING REGISTERED!");
  Serial.println("   Type: " + String(buildingType));
  Serial.println("   UID: " + uid);
  Serial.println("   Total in database: " + String(buildingRegistry.getDatabaseSize()));
  
  // Debug: Check if building is actually in database
  if (buildingRegistry.hasBuilding(uid)) {
    Serial.println("   ✅ Confirmed: Building is in database");
  } else {
    Serial.println("   ❌ ERROR: Building NOT found in database after adding!");
  }
  Serial.println();
}

void onDeleteBuilding(uint8_t buildingType, const String& uid) {
  Serial.println("🗑️ BUILDING DELETED!");
  Serial.println("   Type: " + String(buildingType));
  Serial.println("   UID: " + uid);
  Serial.println("   Remaining in database: " + String(buildingRegistry.getDatabaseSize()));
  
  // Debug: Check if building is actually removed from database
  if (!buildingRegistry.hasBuilding(uid)) {
    Serial.println("   ✅ Confirmed: Building removed from database");
  } else {
    Serial.println("   ❌ ERROR: Building STILL in database after deletion!");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== NFC Building Registry Test ===");
  Serial.println("Library version: 1.0.0");
  
  // Initialize SPI bus
  SPI.begin();
  
  // Initialize MFRC522
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  
  // Set up event callbacks
  buildingRegistry.setOnNewBuildingCallback(onNewBuilding);
  buildingRegistry.setOnDeleteBuildingCallback(onDeleteBuilding);
  
  Serial.println();
  Serial.println("Available commands:");
  Serial.println("  'd' - Toggle delete mode (currently: " + String(buildingRegistry.isDeleteMode() ? "ON" : "OFF") + ")");
  Serial.println("  'c' - Clear building database");
  Serial.println("  'p' - Print all buildings");
  Serial.println("  's' - Show statistics");
  Serial.println("  'h' - Show this help");
  Serial.println();
  Serial.println("Ready! Place NFC cards near the reader...");
  Serial.println();
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toLowerCase();
    
    if (command == "d") {
      buildingRegistry.setDeleteMode(!buildingRegistry.isDeleteMode());
      Serial.println("Delete mode: " + String(buildingRegistry.isDeleteMode() ? "ON" : "OFF"));
    }
    else if (command == "c") {
      buildingRegistry.clearDatabase();
      Serial.println("✅ Database cleared!");
    }
    else if (command == "p") {
      buildingRegistry.printDatabase();
    }
    else if (command == "s") {
      showStatistics();
    }
    else if (command == "h") {
      showHelp();
    }
    else if (command.length() > 0) {
      Serial.println("Unknown command: '" + command + "'. Type 'h' for help.");
    }
    Serial.println();
  }
  
  // Scan for NFC cards
  buildingRegistry.scanForCards();
  
  delay(100); // Prevent flooding
}

void showStatistics() {
  Serial.println("=== Building Database Statistics ===");
  Serial.println("Total buildings: " + String(buildingRegistry.getDatabaseSize()));
  Serial.println("Delete mode: " + String(buildingRegistry.isDeleteMode() ? "ENABLED" : "DISABLED"));
  
  if (buildingRegistry.getDatabaseSize() > 0) {
    // Count buildings by type
    auto allBuildings = buildingRegistry.getAllBuildings();
    std::map<uint8_t, int> typeCounts;
    
    for (const auto& pair : allBuildings) {
      uint8_t type = pair.second.buildingType;
      typeCounts[type]++;
    }
    
    Serial.println("\nBuildings by type:");
    for (const auto& pair : typeCounts) {
      Serial.println("  Type " + String(pair.first) + ": " + String(pair.second) + " building" + (pair.second == 1 ? "" : "s"));
    }
    
    // Show unique building types
    Serial.println("\nUnique building types: " + String(typeCounts.size()));
  } else {
    Serial.println("Database is empty.");
  }
  Serial.println("====================================");
}

void showHelp() {
  Serial.println("=== Available Commands ===");
  Serial.println("  'd' - Toggle delete mode");
  Serial.println("  'c' - Clear building database");
  Serial.println("  'p' - Print all registered buildings");
  Serial.println("  's' - Show database statistics");
  Serial.println("  'h' - Show this help message");
  Serial.println();
  Serial.println("How it works:");
  Serial.println("• Scan NFC cards to register buildings");
  Serial.println("• Each card represents a building type");
  Serial.println("• Same card won't be added twice");
  Serial.println("• Enable delete mode to remove buildings");
  Serial.println("• Building types are read from NDEF or derived from UID");
  Serial.println("==========================");
}
