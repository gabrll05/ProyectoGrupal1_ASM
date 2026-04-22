#include <SPI.h>
#include <arduinoFFT.h>
#include <math.h>

#define N 32
#define K 3
#define PWM_PIN 9

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 8000);

volatile byte data;
volatile bool flag = false;

byte buffer[K * 5];
byte idx = 0;

float audioBuffer[N];
int audioIndex = 0;

void setup() {
  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);
  pinMode(PWM_PIN, OUTPUT);

  // PWM rápido
  TCCR1B = TCCR1B & 0b11111000 | 0x01;

  SPCR = _BV(SPE);
  SPI.attachInterrupt();
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {

  // recepción SPI
  while (flag) {
    buffer[idx++] = data;

    if (idx >= K * 5) {
      idx = 0;

      // limpiar espectro
      for (int i = 0; i < N; i++) {
        vReal[i] = 0;
        vImag[i] = 0;
      }

      // reconstruir con fase
      for (int i = 0; i < K; i++) {

        int base = i * 5;

        int k = buffer[base];

        int16_t real = (buffer[base+1] << 8) | buffer[base+2];
        int16_t imag = (buffer[base+3] << 8) | buffer[base+4];

        vReal[k] = real;
        vImag[k] = imag;

        if (k > 0 && k < N) {
          vReal[N-k] = real;
          vImag[N-k] = -imag;
        }
      }

      // IFFT
      FFT.compute(FFTDirection::Reverse);

      for (int i = 0; i < N; i++) {
        audioBuffer[i] = vReal[i] / N;
      }
    }

    flag = false;
  }

  // reproducción continua
  float sample = audioBuffer[audioIndex];

  int pwm = (int)(sample * 2 + 127);

  if (pwm < 0) pwm = 0;
  if (pwm > 255) pwm = 255;

  analogWrite(PWM_PIN, pwm);

  audioIndex++;
  if (audioIndex >= N) audioIndex = 0;

  delayMicroseconds(125);
}