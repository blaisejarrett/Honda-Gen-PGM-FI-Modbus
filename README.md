# Honda-Gen-PGM-FI-Modbus
Adapter to gain access to Honda Gen DLC data via Modbus TCP.

**The current master branch of this project only offers up mock values and does not query the actual hardware interface. This project is still in development.**

See my other related projects for more context.
- [Honda-Gen-PGM-FI-Interface](https://github.com/blaisejarrett/Honda-Gen-PGM-FI-Interface)
- [Honda-Gen-ESPHome](https://github.com/blaisejarrett/Honda-Gen-ESPHome)
- [Honda-Gen-PGM-FI-Protocol](https://github.com/blaisejarrett/Honda-Gen-PGM-FI-Protocol)


# Modbus Table

The values are broken up into their respective underlying controllers. These numbers are 0-indexed. If your Modbus client is 1-indexed. Add 1 to the addresses. All values are unsigned unless noted.

### ECU1
| Address | Description | Example | Notes |
| ------- | ----------- | ------- | ----- |
| 100     | Runtime in Days | 42 | |
| 101     | Runtime in Hours | 59 | |
| 102     | Runtime in Minutes | 30 | |
| 103     | Engine RPM | 3000 | |
| 104     | Battery voltage * 10 | 124 | Divide by 10 to get 12.4v for example
| 105     | Cylinder Head Temperature in C | 50 | This is a signed integer |
| 106     | Ignition Degrees | 65526 | Signed integer, -10 for example |
| 107     | Output Voltage | 120 | |

### ECU2
| Address | Description | Example | Notes |
| ------- | ----------- | ------- | ----- |
| 110     | Baro Pressure in KPA | 100 | |
| 111     | Fuel Injection Time in MS * 100 | 410 | Divide by 100 to get the real value. Ex) 4.10 |
| 112     | Throttle Position in % | 80 | |
| 113     | Overall Output Power in VA | 4000 | |
| 114     | Starter Count | 10 | |
| 115     | o2 sensor in V * 10 | 12 | Divide by 10 to get 1.2v for example |

### Inverter 1
| Address | Description | Example | Notes |
| ------- | ----------- | ------- | ----- |
| 120     | Amps * 10 | 300 | Divide by 10 to get real amps. 30a for example. |
| 121     | Temperature in celsius | 30 | Signed integer |
| 122     | Mode | 3 | This value is unknown. Suspected to indicate eco mode but unsure. |
| 123     | Output watts | 3000 | |
| 124     | Output volts | 120 |

### Inverter 2 
| Address | Description | Example | Notes |
| ------- | ----------- | ------- | ----- |
| 130     | Amps * 10 | 300 | Divide by 10 to get real amps. 30a for example. |
| 131     | Temperature in celsius | 30 | Signed integer |
| 133     | Output watts | 3000 | |