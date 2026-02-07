#include <Arduino.h>

int cs_pin = 2; // slave select aka cs
int clk_pin = 3;
int mosi_pin = 4; // master out slave in aka di
int miso_pin = 5; // master in slave out aka do

int clock_period = 5;

void setClock(uint8_t val) {
  digitalWrite(clk_pin, val);
}

void transferBit(uint8_t mosiBit) {

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
  for (int i = 0; i < 8; i++) {
    // Clock is low, prepare data for chip
    digitalWrite(mosi_pin, hex_9F[i]);
    delayMicroseconds(clock_period);

    setClock(HIGH);
    delayMicroseconds(clock_period);
    // data has been sampled by chip, set low
    setClock(LOW);
    delayMicroseconds(clock_period);
  }

  // read 3 bytes 
  for (int j = 0; j < 3; j++) {
    Serial.println("OUTPUT BYTE:");
    for (int i = 0; i < 8; i++) {

      // Clock is low present dummy bit
      digitalWrite(mosi_pin, LOW);
      delayMicroseconds(clock_period);

      setClock(HIGH);
      delayMicroseconds(clock_period);

      // reads miso after rising clock
      uint8_t t1 = digitalRead(miso_pin);
      Serial.print(t1);

      setClock(LOW);
      delayMicroseconds(clock_period);
    }
    Serial.println("");
  }

  digitalWrite(cs_pin, HIGH);
}

void spi_step() {}

void setup() {
  pinMode(cs_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT_PULLUP); // minimal change: helps when MISO floats

  Serial.begin(9600);

  // hello world test, read some data from the board
  initialize();
}

void loop() {

}

