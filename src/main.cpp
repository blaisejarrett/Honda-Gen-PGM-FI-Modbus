#include <Arduino.h>
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>
#include <pgmfi_decoder.h>

ModbusIP mb;
WiFiManager wm;

struct PGM_Base {
  unsigned long ecu1_last_q_time;
  const uint16_t mb_addr = 0;
  const uint16_t len = 0;
  uint16_t * values; 

  PGM_Base(uint16_t mb_addr_, uint16_t len_): 
    mb_addr(mb_addr_), len(len_) {
      this->values = (uint16_t *)malloc(sizeof(uint16_t) * this->len);
  }

  virtual uint16_t getValue(uint16_t address);

  uint16_t getValue(TRegister* reg) {
    uint16_t addr = reg->address.address - this->mb_addr;
    return this->getValue(addr);
  };
};

// A place to store all the data from the generator.
struct MB_ECU_Info1 : DLC::ECU_Info1, PGM_Base {
  MB_ECU_Info1() : PGM_Base(100, 8) {}
  
  using PGM_Base::getValue;
  virtual uint16_t getValue(uint16_t address) {
    return 40 + address;
  };
} ecu1;

// DLC::ECU_Info2 ecu2;       // 6
// DLC::Inverter_Master inv1; // 5
// DLC::Inverter_Slave inv2;  // 3



void setup() {
  Serial.begin(115200);

  if (wm.autoConnect("PGMFI-Modbus", "modbus123"))
    Serial.println("Connected to WiFi!");
  else
    Serial.println("Failed to connect.");

  mb.server();

  // set up ECU info1
  mb.addIreg(ecu1.mb_addr, 0, ecu1.len);
  mb.onGetIreg(ecu1.mb_addr, [](TRegister* reg, uint16_t) {
    return ecu1.getValue(reg);
  }, ecu1.len);
}


void loop() {
  // put your main code here, to run repeatedly:
  mb.task();
}
