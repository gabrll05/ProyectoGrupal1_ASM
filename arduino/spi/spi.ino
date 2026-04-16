#include <SPI.h>

#define CS1 10
#define CS2 9

void setup() {
  Serial.begin(115200);

  pinMode(CS1, OUTPUT);
  pinMode(CS2, OUTPUT);

  digitalWrite(CS1, HIGH);
  digitalWrite(CS2, HIGH);

  SPI.begin();
  SPI.beginTransaction(SPISettings(50000, MSBFIRST, SPI_MODE0));
}

void loop() {
  if (Serial.available()) {

    String msg = Serial.readStringUntil('\n');

    SPI.begin();

    digitalWrite(CS1, LOW);
    delayMicroseconds(10);

    for (int i = 0; i < msg.length(); i++) {
      SPI.transfer(msg[i]);
    }

    delayMicroseconds(10);
    digitalWrite(CS1, HIGH);

    digitalWrite(CS2, LOW);
    delayMicroseconds(10);

    for (int i = 0; i < msg.length(); i++) {
      SPI.transfer(msg[i]);
    }

    delayMicroseconds(10);
    digitalWrite(CS2, HIGH);

    SPI.end();
  }
}