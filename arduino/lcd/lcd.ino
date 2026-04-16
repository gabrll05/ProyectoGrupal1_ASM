#include <SPI.h>

volatile byte data;
volatile bool flag = false;

void setup() {
  Serial.begin(115200);

  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP); // SS estable

  SPCR = _BV(SPE);           // habilitar SPI
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {
  if (flag) {
    Serial.write(data);
    flag = false;
  }
}