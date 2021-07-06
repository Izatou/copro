# sensor-coprocessor

### WARNING
1. Connect all power 5V and 3.3V as necessary, with common ground
2. Sensorbox needs 5V and 3.3V to power the ADS1256

### Wiring Diagram
```
Atmega328p      Raspi
GND       ->    GND 
A4 (PC4)  ->    SDA (3, GPIO2)
A5 (PC5)  ->    SCL (5, GPIO3)


SensorBox        Atmega328p
SDA (3)    ->    D6  (PD6)
SCL (5)    ->    D7  (PD7)
SCK (23)   ->    D13 (PB5, SCK)
MISO(21)   ->    D12 (PB4, MISO)
MOSI(19)   ->    D11 (PB3, MOSI)
CS0 (15,P3)->    D10 (PB2)
DRDY(11,P0)->    D9  (PB1)
RST (12,P1)->	 D8  (PB0)
```
