#include <SPI.h>
#include <math.h>

#define PWM_PIN 9

volatile byte data;
volatile bool flag = false;

byte buffer[6];
byte idx = 0;

float freq[3] = {0,0,0};
float amp[3] = {0,0,0};
float phase[3] = {0,0,0};

void setup() {
  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT);

  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  SPCR = _BV(SPE);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {

  while (flag) {
    buffer[idx++] = data;

    if (idx >= 6) {
      idx = 0;

      for (int i = 0; i < 3; i++) {
        freq[i] = buffer[i*2] * 15.0;
        amp[i]  = buffer[i*2 + 1] / 255.0;
      }
    }

    flag = false;
  }

  float sample = 0;

  for (int i = 0; i < 3; i++) {
    sample += amp[i] * 40 * sin(phase[i]);
  }

  int pwm = (int)(127 + sample);

  if (pwm < 0) pwm = 0;
  if (pwm > 255) pwm = 255;

  analogWrite(PWM_PIN, pwm);

  for (int i = 0; i < 3; i++) {
    phase[i] += 2 * PI * freq[i] / 8000.0;
  }

  delayMicroseconds(60);
}