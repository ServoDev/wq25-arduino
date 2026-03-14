#include <Arduino.h>
#include <SPI.h>

const uint8_t CS_PIN = 10;
const uint8_t HW_SS_PIN = 10;
const uint32_t FLASH_CAPACITY_BYTES = 2UL * 1024UL * 1024UL; // total storage size
const uint32_t SPI_CLOCK_HZ = 1000000UL;

const uint8_t CMD_READ_DATA = 0x03;
const uint8_t CMD_WRITE_ENABLE = 0x06;
const uint8_t CMD_READ_STATUS_1 = 0x05;
const uint8_t CMD_JEDEC_ID = 0x9F;

SPISettings flashSpiSettings(SPI_CLOCK_HZ, MSBFIRST, SPI_MODE0);

void spiStart() {
  SPI.beginTransaction(flashSpiSettings);
  digitalWrite(CS_PIN, LOW);
}

void spiEnd() {
  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

uint8_t transferByte(uint8_t outByte) {
  return SPI.transfer(outByte);
}

void sendAddress24(uint32_t address) {
  // W25Q16BV commands use a 24-bit address sent most-significant byte first.
  transferByte((address >> 16) & 0xFF);
  transferByte((address >> 8) & 0xFF);
  transferByte(address & 0xFF);
}

void printHexByte(uint8_t value) {
  if (value < 0x10) {
    Serial.print('0');
  }

  Serial.print(value, HEX);
}

uint8_t readStatusRegister1() {
  spiStart();
  transferByte(CMD_READ_STATUS_1);
  uint8_t status = transferByte(0x00);
  spiEnd();
  return status;
}

void writeEnable() {
  spiStart();
  transferByte(CMD_WRITE_ENABLE);
  spiEnd();
}

void readJedecId(uint8_t *manufacturerId, uint8_t *memoryType, uint8_t *capacity) {
  spiStart();
  transferByte(CMD_JEDEC_ID);
  *manufacturerId = transferByte(0x00);
  *memoryType = transferByte(0x00);
  *capacity = transferByte(0x00);
  spiEnd();
}

bool readData(uint32_t address, uint8_t *buffer, size_t count) {
  // invalid reads check.
  if (buffer == NULL) {
    return false;
  }

  if (address >= FLASH_CAPACITY_BYTES) {
    return false;
  }

  if (count > (FLASH_CAPACITY_BYTES - address)) {
    return false;
  }

  spiStart();
  transferByte(CMD_READ_DATA);
  sendAddress24(address);

  // after the command and address, each dummy byte clocks out one data byte.
  for (size_t index = 0; index < count; index++) {
    buffer[index] = transferByte(0x00);
  }

  spiEnd();
  return true;
}

void printBuffer(const uint8_t *buffer, size_t count) {
  // print bytes as space separated hex
  for (size_t index = 0; index < count; index++) {
    if (index > 0) {
      Serial.print(' ');
    }

    printHexByte(buffer[index]);
  }

  Serial.println();
}

void runStartupDemo() {
  uint8_t manufacturerId = 0;
  uint8_t memoryType = 0;
  uint8_t capacity = 0;
  uint8_t readBuffer[16];

  readJedecId(&manufacturerId, &memoryType, &capacity);

  Serial.println("demo");
  Serial.print("JEDEC ID: ");
  printHexByte(manufacturerId);
  Serial.print(' ');
  printHexByte(memoryType);
  Serial.print(' ');
  printHexByte(capacity);
  Serial.println();

  Serial.print("Status Register-1: 0x");
  printHexByte(readStatusRegister1());
  Serial.println();

  if (readData(0x000000, readBuffer, sizeof(readBuffer))) {
    Serial.print("Read @0x000000: ");
    printBuffer(readBuffer, sizeof(readBuffer));
  } else {
    Serial.println("Read failed: address out of range");
  }

  writeEnable();
  Serial.print("Status after WREN: 0x");
  printHexByte(readStatusRegister1());
  Serial.println();
}

void setup() {
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  pinMode(HW_SS_PIN, OUTPUT);
  digitalWrite(HW_SS_PIN, HIGH);

  Serial.begin(9600);
  Serial.setTimeout(10);
  delay(50);

  SPI.begin();
  runStartupDemo();
}

void loop() {
}
