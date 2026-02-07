#line 1 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
#include <Arduino.h>

uint8_t cs_pin = 2; // slave select aka cs
uint8_t clk_pin = 3;
uint8_t mosi_pin = 4; // master out slave in aka di
uint8_t miso_pin = 5; // master in slave out aka do

int clock_period = 5;

#line 10 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void setClock(int val);
#line 14 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void transferBit(uint8_t mosiBit);
#line 20 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void initialize();
#line 81 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void spi_step();
#line 85 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void setup();
#line 98 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void loop();
#line 10 "/mnt/c/dev/wq25-arduino/driver/driver.ino"
void setClock(int val) {
  digitalWrite(clk_pin, val);
}

void transferBit(uint8_t mosiBit) {


}

// activate cs pin
void initialize() {
  // set the clock high before setting cs, this puts the board
  // into spi mode 3
  setClock(HIGH);
  delayMicroseconds(clock_period);

  // to acrivate, cs needs to go from high to low
  digitalWrite(cs_pin, HIGH);
  delayMicroseconds(clock_period);
  digitalWrite(cs_pin, LOW);

  // wait for the device to be ready
  delayMicroseconds(clock_period);

  // // jsut added this
  // setClock(LOW);
  // delayMicroseconds(clock_period);

  // test 0x05 - read status reg
  int hex_92[8] = { 0, 0, 0, 0, 0, 1, 0, 1 }; // msb at index 0
  for (int i=0; i<8; i++) {
    // send data to chip
    digitalWrite(mosi_pin, hex_92[i]);
    delayMicroseconds(clock_period);

    setClock(LOW);
    delayMicroseconds(clock_period);
    setClock(HIGH);
    delayMicroseconds(clock_period);
    // get data from chip
    uint8_t t1 = digitalRead(miso_pin);
    Serial.println(t1);

    delayMicroseconds(clock_period);
  }

  // read device id fr now
  for (int j=0; j<2; j++) {
    Serial.println("OUTPUT:");
    for (int i=0; i<8; i++) {
      digitalWrite(mosi_pin, LOW);
      delayMicroseconds(clock_period);
      setClock(LOW);
      delayMicroseconds(clock_period);

      setClock(HIGH);
      delayMicroseconds(clock_period);

      uint8_t t1 = digitalRead(miso_pin);
      Serial.print(t1);

      delayMicroseconds(clock_period);
      // send data to chip
    }
    Serial.println("");
  }


  digitalWrite(cs_pin, HIGH);
}

void spi_step() {

}

void setup() {
  pinMode(cs_pin, OUTPUT);
  pinMode(clk_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);
  pinMode(miso_pin, INPUT);

  Serial.begin(9600);

  // hello world test, read some data from the board
  initialize();

}

void loop() {

}

