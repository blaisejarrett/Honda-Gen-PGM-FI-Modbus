#include <Arduino.h>
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>
#include <pgmfi_decoder.h>

ModbusIP mb;
WiFiManager wm;

struct PGM_Base {
  const unsigned long expiration_ms = 1000;
  unsigned long last_q_time;
  const uint16_t mb_addr = 0;
  const uint16_t len = 0;
  uint16_t * values; 

  PGM_Base(uint16_t mb_addr_, uint16_t len_): 
    mb_addr(mb_addr_), len(len_) {
      this->values = (uint16_t *)malloc(sizeof(uint16_t) * this->len);
  }

  virtual void queryHardware() {}
  virtual void populateValues() {}

  void update() {
    this->queryHardware();
    this->populateValues();  
  };

  uint16_t getValue(uint16_t address) {
    unsigned long current_t = millis();
    if (this->last_q_time + this->expiration_ms < current_t) {
      // we have expired. Make a new query
      this->update();
      this->last_q_time = millis();
    }

    // sanity check for array out of bounds
    if (address >= this->len)
      return 0;
    
    return this->values[address];
  }

  uint16_t getValue(TRegister* reg) {
    uint16_t addr = reg->address.address - this->mb_addr;
    return this->getValue(addr);
  };
};

struct MB_ECU_Info1 : DLC::ECU_Info1, PGM_Base {
  MB_ECU_Info1() : PGM_Base(100, 8) {}
  
  virtual void queryHardware() {
    // for testing purposes, we'll make up values instead of querying the hardware

    // runtime in days
    this->runtime_d = 42;
    // runtime in hours
    this->runtime_h = 59;
    // runtime in minutes
    this->runtime_m = 30;
    
    this->rpm = 3000;
    // voltage in 10th of a volt. ex 121 is 12.1 volts.
    this->battery_voltage = (uint8_t)random(120, 130);
    // degrees celsius
    this->cylinder_head_temp = (int16_t)random(50, 60);
    // degrees
    this->ignition_degrees = -10;
    // voltage
    this->output_voltage = 121;
  }

  virtual void populateValues() {
    this->values[0] = this->runtime_d;
    this->values[1] = this->runtime_h;
    this->values[2] = this->runtime_m;
    this->values[3] = this->rpm;
    this->values[4] = this->battery_voltage;
    this->values[5] = (uint16_t)this->cylinder_head_temp;
    this->values[6] = (uint16_t)this->ignition_degrees;
    this->values[7] = this->output_voltage;
  }
} ecu1;

struct MB_ECU_Info2 : DLC::ECU_Info2, PGM_Base {
  MB_ECU_Info2() : PGM_Base(110, 6) {}
  
  virtual void queryHardware() {
    // for testing purposes, we'll make up values instead of querying the hardware
    // Pressure in KPA
    this->barometric_pressure = 100;
    // * 100 in milliseconds. divide by 100 to get real ms value
    this->fuel_injection_time = (uint16_t)random(300, 400);
    // Position in %
    this->throttle_position = 80;
    // Power in VA
    this->output_power = (uint16_t)random(4000, 4400);
    // Count.
    this->starter_count = 132;
    // sensor value in 0.1 volts
    this->o2 = 10;
  }

  virtual void populateValues() {
    this->values[0] = this->barometric_pressure;
    this->values[1] = this->fuel_injection_time;
    this->values[2] = this->throttle_position;
    this->values[3] = this->output_power;
    this->values[4] = this->starter_count;
    this->values[5] = this->o2;
  }
} ecu2;

struct MB_Inverter_Master : DLC::Inverter_Master, PGM_Base {
  MB_Inverter_Master() : PGM_Base(120, 5) {}
  
  virtual void queryHardware() {
    // for testing purposes, we'll make up values instead of querying the hardware
    // Amps * 10. Divide by 10 to get real amps.
    this->amperage = (3000/120) * 10;
    // Temperature in celsius
    this->temperature = 10;
    // Mode. 2 is eco? 3 is not eco?
    this->mode = 3;
    // Output watts.
    this->watts = (uint16_t)random(3000, 3400);
    // Output volts.
    this->voltage = 124;
  }

  virtual void populateValues() {
    this->values[0] = this->amperage;
    this->values[1] = (uint16_t)this->temperature;
    this->values[2] = this->mode;
    this->values[3] = this->watts;
    this->values[4] = this->voltage;
  }
} inverter1;

struct MB_Inverter_Slave : DLC::Inverter_Slave, PGM_Base {
  MB_Inverter_Slave() : PGM_Base(130, 3) {}
  
  virtual void queryHardware() {
    // for testing purposes, we'll make up values instead of querying the hardware
    // Amps * 10. Divide by 10 to get real amps.
    this->amperage = (1300/120) * 10;
    // Temperature in celsius
    this->temperature = 11;
    // Output watts.
    this->watts = (uint16_t)random(1200, 1400);;
  }

  virtual void populateValues() {
    this->values[0] = this->amperage;
    this->values[1] = (uint16_t)this->temperature;
    this->values[2] = this->watts;
  }
} inverter2;



void setup() {
  Serial.begin(115200);

  if (wm.autoConnect("PGMFI-Modbus", "modbus123"))
    Serial.println("Connected to WiFi!");
  else
    Serial.println("Failed to connect.");

  mb.server();

  PGM_Base * ecu_controllers[] = {&ecu1, &ecu2, &inverter1, &inverter2};
  size_t ecu_n = sizeof(ecu_controllers) / sizeof(PGM_Base*);

  for (size_t i = 0; i < ecu_n; i++) {
    PGM_Base * controller = ecu_controllers[i];
    mb.addIreg(controller->mb_addr, 0, controller->len);
    mb.onGetIreg(controller->mb_addr, [controller](TRegister* reg, uint16_t) {
      return controller->getValue(reg);
    }, controller->len);
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  mb.task();
}
