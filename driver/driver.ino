#include <Arduino.h>

uint8_t ss_pin = 2; // slave select aka cs
uint8_t clk_pin = 3;
uint8_t mosi_pin = 4; // master out slave in aka di
uint8_t miso_pin = 5; // master in slave out aka do

int clock_period = 5;

void setClock(int val) {
  digitalWrite(clk_pin, val);
}

// activate cs pin
void init() {
  digitalWrite(ss_pin, HIGH);
  delay_ms()
}

void setup() {
  pinMode(ss_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT);

  // hello world test, read some data from the boad

}

void loop() {

}
