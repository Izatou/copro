//Install all necessary library first!
#include <ADS1256.h>
#include <SPI.h>
#include <Wire.h>

#include <SoftWire.h>
#include <AsyncDelay.h>

#define SELF_ADDRESS 0x62
#define SHT31_ADDRESS 0x44

#define ADS1256_CLOCK_MHZ 7.68
#define ADS1256_VREF 2.5 // VREF = VIN / 2; VIN -> 5V

#define CHANNEL_NUM 11
#define CHUNK_NUM 22 // 11 channel x 4 bytes = 44 bytes / 2 chunk = 22 bytes per chunk

ADS1256 adc(ADS1256_CLOCK_MHZ, ADS1256_VREF, false);

SoftWire sw(6, 7); // SDA 6, SCL 7 (Master)
char swTxBuffer[41];
char swRxBuffer[41];

AsyncDelay readInterval;

float data[CHANNEL_NUM];
uint8_t dataReadOffset = 0;

void setup()
{
  // Serial.begin(9600);

  adc.begin(ADS1256_DRATE_30000SPS, ADS1256_GAIN_1, false);

  sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
  sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
  sw.setDelay_us(5);
  sw.setTimeout(1000);
  sw.begin();

  Wire.begin(SELF_ADDRESS);
  Wire.onRequest(onI2CRead);
  Wire.onReceive(onI2CWrite);

  readInterval.start(500, AsyncDelay::MILLIS);
}

float readSensor(uint8_t i)
{  
  adc.waitDRDY();
  adc.setChannel(i);
  adc.waitDRDY();

  delay(10);

  unsigned long long c = (unsigned long long)adc.readCurrentChannelRaw() * 1023 / 0x7fffff;
  return (float) c;
}

void readSHT31()
{
  //0x2c06: High Repeatability. See Datasheet pg.10
  sw.beginTransmission(SHT31_ADDRESS);
  sw.write(0x2c);
  sw.write(0x06);
  sw.endTransmission();
  delay(16);

  uint8_t buffer[6];
  int numBytes = sw.requestFrom(SHT31_ADDRESS, 6);
  for (uint8_t i = 0; i < 6; i++) {
    buffer[i] = sw.read();
  }
  if (numBytes != 6) {
    return;
  }

  uint16_t raw = (buffer[0] << 8) + buffer[1];
  float temperature = raw * (175.0 / 65535) - 45;
  raw = (buffer[3] << 8) + buffer[4];
  float humidity = raw * (100.0 / 65535);

  data[9] = temperature;
  data[10] = humidity;
}

void acquireData()
{
  // Serial.println("==");
  // Acquire Data
  for (uint8_t i = 0; i < 8; i++)
  {
    data[i] = readSensor(i);
    // Serial.println(data[i]);
  }

  // Acquire SHT31
  readSHT31();
}

void onI2CWrite(int num) {
  switch (num) {
    case 1: // Master is about to read me
      dataReadOffset = Wire.read();
      break;
  }
}

void onI2CRead() {
  byte *b = (byte *)&data;
  Wire.write(b + (dataReadOffset * CHUNK_NUM), CHUNK_NUM);
}

void loop()
{
  if (readInterval.isExpired()) {
    readInterval.restart();
    acquireData();
  }
}
