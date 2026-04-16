#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

#define BUFFER_SIZE 32

volatile char buffer[BUFFER_SIZE];
volatile byte head = 0;
volatile byte tail = 0;

byte col = 0;

void setup() {
  Serial.begin(115200);

  lcd.begin(16, 2);
  lcd.clear();

  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);

  SPCR = _BV(SPE);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  char c = SPDR;

  byte next = (head + 1) % BUFFER_SIZE;

  if (next != tail) {
    buffer[head] = c;
    head = next;
  }
}

void loop() {
  while (tail != head) {
    char c = buffer[tail];
    tail = (tail + 1) % BUFFER_SIZE;

    Serial.write(c);

    lcd.setCursor(col, 0);
    lcd.write(c);

    col++;

    if (col >= 16) {
      col = 0;
      lcd.clear();
    }
  }
}