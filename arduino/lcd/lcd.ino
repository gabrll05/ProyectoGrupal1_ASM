#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(2,3,4,5,6,7);

#define BINS 16

volatile byte data;
volatile bool flag = false;

byte spectrum[BINS];
byte index = 0;

byte bars[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,31},
  {0,0,0,0,0,0,31,31},
  {0,0,0,0,0,31,31,31},
  {0,0,0,0,31,31,31,31},
  {0,0,0,31,31,31,31,31},
  {0,0,31,31,31,31,31,31},
  {0,31,31,31,31,31,31,31}
};

void setup() {
  lcd.begin(16,2);
  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, bars[i]);
  }
  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  SPCR = _BV(SPE);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {
  if (flag) {
    spectrum[index++] = data;

    if (index >= BINS) {
      index = 0;

      byte maxVal = 1;
      for (int i = 0; i < 16; i++) {
        if (spectrum[i] > maxVal) maxVal = spectrum[i];
      }

      for (int i = 0; i < 16; i++) {
        byte level = (spectrum[i] * 15) / maxVal;

        byte bottom = min(level, 7);
        byte top = (level > 7) ? (level - 7) : 0;

        lcd.setCursor(i, 1);
        lcd.write(bottom);

        lcd.setCursor(i, 0);
        lcd.write(top);
      }
    }

    flag = false;
  }
}