#include <Arduino.h>
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>

ModbusIP mb;
WiFiManager wm;

const int TEST_HREG = 100;


void setup() {
  Serial.begin(115200);

  if (wm.autoConnect("PGMFI-Modbus", "modbus123"))
    Serial.println("Connected to WiFi!");
  else
    Serial.println("Failed to connect.");

  mb.server();
  mb.addHreg(TEST_HREG, 0xABCD);
}


void loop() {
  // put your main code here, to run repeatedly:
  mb.task();
}

