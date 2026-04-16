#include <SPI.h>
#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(2,3,4,5,6,7);

#define BINS 16
#define PWM_PIN 9

volatile byte data;
volatile bool flag = false;

byte spectrum[BINS];
byte idx = 0;

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

float phase1 = 0;
float phase2 = 0;

void setup() {
  lcd.begin(16,2);
  for (int i = 0; i < 8; i++) lcd.createChar(i, bars[i]);

  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT);

  SPCR = _BV(SPE);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {

  if (flag) {
    spectrum[idx++] = data;

    if (idx >= BINS) {
      idx = 0;

      byte maxVal = 1;
      int maxIndex = 0;
      int secondIndex = 1;

      for (int i = 0; i < 16; i++) {
        if (spectrum[i] > maxVal) {
          maxVal = spectrum[i];
          maxIndex = i;
        }
      }

      for (int i = 0; i < 16; i++) {
        if (i != maxIndex && spectrum[i] > spectrum[secondIndex]) {
          secondIndex = i;
        }
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

      float freq1 = maxIndex * 50.0;
      float freq2 = secondIndex * 50.0;

      for (int t = 0; t < 100; t++) {
        float sample =
          100 * sin(phase1) +
          80 * sin(phase2);

        int pwm = (int)(127 + sample);
        if (pwm < 0) pwm = 0;
        if (pwm > 255) pwm = 255;

        analogWrite(PWM_PIN, pwm);

        phase1 += 2 * PI * freq1 / 8000.0;
        phase2 += 2 * PI * freq2 / 8000.0;

        delayMicroseconds(125);
      }
    }

    flag = false;
  }
}