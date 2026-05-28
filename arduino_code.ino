#include <SPI.h>
#include <SD.h>

// Configuration
const int chipSelect = 10;
const String CMD_START_FILENAME = "START_FILE:";
const String CMD_START_DATA = "START_DATA:";
const String CMD_END_FILE = "END_FILE";

// State Machine
enum State { IDLE, GET_FILENAME, GET_DATA };
State currentState = IDLE;

File dataFile;
String currentFileName = "";

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; // Wait for serial port to connect
  }

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done. Waiting for commands...");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove whitespace/newlines

    if (input.startsWith(CMD_START_FILENAME)) {
      currentFileName = input.substring(CMD_START_FILENAME.length());
      currentState = GET_FILENAME;
      Serial.print("Creating file: ");
      Serial.println(currentFileName);
    } 
    
    else if (input.startsWith(CMD_START_DATA)) {
      if (currentFileName != "") {
        dataFile = SD.open(currentFileName, FILE_WRITE);
        if (dataFile) {
          currentState = GET_DATA;
          Serial.println("Ready for data...");
        } else {
          Serial.println("Error opening file!");
        }
      }
    } 
    
    else if (input == CMD_END_FILE) {
      if (dataFile) {
        dataFile.close();
        Serial.println("File saved and closed.");
      }
      currentState = IDLE;
      currentFileName = "";
    } 
    
    else {
      // Logic for handling the actual data stream
      if (currentState == GET_DATA && dataFile) {
        dataFile.println(input);
        Serial.println("Writing line...");
      }
    }
  }
}
