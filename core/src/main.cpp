#include <Arduino.h>
#include <EEPROM.h>
#include <Shrike.h>

ShrikeFlash fpga;

void setup() {
  delay (2000);
  Serial.begin (115200);
  while (!Serial) {
    delay (10);
  }

  Serial.println ("Shrike Flash Example");
  
  // Initialize the library
  if (!fpga.begin()) {
    Serial.println ("Initialization failed!");
    while (1) {
      Serial.println ("FPGA is not running!");
    }
  }
  
  // Flash the FPGA
  Serial.print ("Flashing FPGA..");
  fpga.flash ("/led_blink.bin");
  Serial.println (" Done.");
}

void loop() {
  // Your code here
  Serial.println ("FPGA is running!");
  delay (2000);
}