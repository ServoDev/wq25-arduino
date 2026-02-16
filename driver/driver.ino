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
uint8_t hexToInt(String str) {
  int ret = 0;
  for (int i=0; i<2; i++) {
    ret *= 16; // treats left digit as 16s place hex

    char ch = str.charAt(i);

    // check if in bounds for hex
    // add value if so
    if (isdigit(ch)) {
      ret += ch - '0';
    }
    else if ((ch >= 'a' && ch <= 'f')) {
      ret += 10+(ch - 'a');
    } else if (ch >= 'A' && ch <= 'F') {
      ret += 10+(ch - 'A');
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


// call before sending op
void spiStart() {
  // run with spi mode 0
  digitalWrite(cs_pin, HIGH);
  setClock(LOW);
  digitalWrite(mosi_pin, LOW);

  delayMicroseconds(clock_period);

  // to activate, cs needs to go from high to low
  digitalWrite(cs_pin, LOW);
  delayMicroseconds(clock_period);

  // start loops with clock low
  setClock(LOW);
  delayMicroseconds(clock_period);
}

// call after done with op
void spiEnd() {
  digitalWrite(cs_pin, HIGH);
}

// loop where operations can be made from serial monitor
void interactive() {
  while (1) {
    spiStart();

    // get write count 
    Serial.println("Enter number of bytes to write");
    while (Serial.available() == 0);
    String str_count = Serial.readString();
    int count = str_count.toInt();

    // write
    for (int i=0; i<count; i++) {
      // get byte in string form
      Serial.println("Enter 2 digit hex value");
      // wait for there to be data
      while (Serial.available() == 0);
      // read the data into board
      String str_byte = Serial.readString();
      // get int val from str in hex format
      uint8_t operation = hexToInt(str_byte);

      // transfer byte
      transferByte(operation);
    }


    // get read count   
    Serial.println("Enter number of bytes to read");
    while (Serial.available() == 0);
    str_count = Serial.readString();
    count = str_count.toInt();


    // read 
    Serial.println("\nOUTPUT: ");
    for (int i=0; i<count; i++) {
      Serial.println(transferByte(0x00));
    }

    spiEnd();
  }
}

// read data at specified addr
void opReadData(int addr, int count) {
  // TODO addr validation

  const int opCode = 0x03;
  transferByte(opCode);

  // turn addr into 3 transfer byte calls
  for (int i=0; i<3; i++) {

  }

  // read the data
  for (int i=0; i<count; i++) {

  }
}

// activate cs pin
void initialize() {

  // interactive();

  spiStart();
  opReadData(0x03a, 10);

  spiEnd();


  // // send 0x9F
  // transferByte(0x9F);
  // // get result
  // Serial.println("\nOUTPUT: ");
  // Serial.println(transferByte(0xFF));
  // Serial.println(transferByte(0xFF));
  // Serial.println(transferByte(0xFF));

}

void setup() {
  pinMode(cs_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT_PULLUP); 

  Serial.begin(9600);
  Serial.setTimeout(10);


  // hello world test, read some data from the board
  initialize();
}

void loop() {

}

