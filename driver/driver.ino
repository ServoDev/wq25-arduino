#include <Arduino.h>

int cs_pin = 2; // slave select aka cs
int clk_pin = 3;
int mosi_pin = 4; // master out slave in aka di
int miso_pin = 5; // master in slave out aka do

int clock_period = 5;

void setClock(uint8_t val) {
  digitalWrite(clk_pin, val);
}

// perform a full bit transfer. returns miso
uint8_t transferBit(uint8_t mosi_bit) {
    // Clock is low, prepare data for chip
    digitalWrite(mosi_pin, mosi_bit);
    delayMicroseconds(clock_period);

    setClock(HIGH);
    delayMicroseconds(clock_period);

    // get miso bit
    uint8_t miso_bit = digitalRead(miso_pin);

    // data has been sampled by chip, set low
    setClock(LOW);
    delayMicroseconds(clock_period);

    return miso_bit;
}

uint8_t transferByte(uint8_t mosi_byte) {
  uint8_t miso_byte = 0;
  for (int i=0; i<8; i++) {
    // Clock is low, prepare data for chip
    digitalWrite(mosi_pin, (mosi_byte >> (7-i)) & 1);
    delayMicroseconds(clock_period);

    setClock(HIGH);
    delayMicroseconds(clock_period);

    // get miso bit
    miso_byte = (miso_byte << 1) | (digitalRead(miso_pin) & 1);

    // data has been sampled by chip, set low
    setClock(LOW);
    delayMicroseconds(clock_period);
  }

    return miso_byte;
}

// activate cs pin
void initialize() {
  // run with spi mode 0
  digitalWrite(cs_pin, HIGH);
  setClock(LOW);
  digitalWrite(mosi_pin, LOW);

  delayMicroseconds(clock_period);

  // to activate, cs needs to go from high to low
  digitalWrite(cs_pin, LOW);
  delayMicroseconds(clock_period);

  // jedec ID command
  int hex_9F[8] = { 1, 0, 0, 1, 1, 1, 1, 1 }; // msb at index 0

  // start loops with clock low
  setClock(LOW);
  delayMicroseconds(clock_period);

  // send 0x9F
  transferByte(0x9f);

  // get result
  Serial.println("\nOUTPUT: ");
  Serial.println(transferByte(0xFF));
  Serial.println(transferByte(0xFF));
  Serial.println(transferByte(0xFF));

  digitalWrite(cs_pin, HIGH);
}

void setup() {
  pinMode(cs_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT_PULLUP); 

  Serial.begin(9600);

  // hello world test, read some data from the board
  initialize();
}

void loop() {

}

