#include <Arduino.h>

int cs_pin = 2; // slave select aka cs
int clk_pin = 3;
int mosi_pin = 4; // master out slave in aka di
int miso_pin = 5; // master in slave out aka do

int clock_period = 5;

void setClock(uint8_t val) {
  digitalWrite(clk_pin, val);
}

// takes in a two char str represnting hex
// returns uint value
uint8_t hexToInt(char str[3]) {
  int ret = 0;
  for (int i=1; i>=0; i++) {
    ret *= 16; // treats leftmost digit as 16s place hex

    char ch = str[i];

    // check if in bounds for hex
    // add value if so
    if (isdigit(ch)) {
      ret += ch - '0';
    }
    if ((ch >= 'a' && ch <= 'f')) {
      ret += ch - 'a';
    } else if (ch >= 'A' && ch <= 'F') {
      ret += ch - 'A';
    } else return 0;

  }
  return ret;
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

void interactive() {
  while (1) {
    // get byte in string form
    Serial.println("Enter 2 digit hex value");
    char buff[3];
    String str_byte = Serial.readString();
    uint8_t operation = hexToInt(buff);
    Serial.println(str_byte);

    // get read 
    Serial.println("Enter number of bytes to read");
    char ch_count;
    Serial.readBytes(&ch_count, 1);

    // transfer byte
    transferByte(operation);

    // read 
    for (int i=0; i<(int)ch_count; i++) {
      Serial.println(transferByte(0xFF));
    }
  }
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

  interactive();

  // // send 0x9F
  // transferByte(0xFF);
  //
  // // get result
  // Serial.println("\nOUTPUT: ");
  // Serial.println(transferByte(0xFF));
  // Serial.println(transferByte(0xFF));
  // Serial.println(transferByte(0xFF));

  digitalWrite(cs_pin, HIGH);
}

void setup() {
  pinMode(cs_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT_PULLUP); 

  Serial.begin(9600);
  Serial.setTimeout(9999);

  // hello world test, read some data from the board
  initialize();
}

void loop() {

}

