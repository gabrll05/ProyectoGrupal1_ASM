#include <arduinoFFT.h>
#include <SPI.h>

#define N 32
#define SS_LCD 10
#define SCALE 8

double vReal[N];
double vImag[N];

double vReal_orig[N];
double vImag_orig[N];

byte original[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 8000);

int index = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  SPI.begin();
  pinMode(SS_LCD, OUTPUT);
  digitalWrite(SS_LCD, HIGH);
}

void loop() {

  if (Serial.available()) {
    byte sample = Serial.read();

    original[index] = sample;

    vReal[index] = sample - 127;
    vImag[index] = 0;
    index++;

    if (index >= N) {
      index = 0;

      // ❌ SIN ventana
      // FFT.windowing(...)

      FFT.compute(FFTDirection::Forward);

      for (int i = 0; i < N; i++) {
        vReal_orig[i] = vReal[i];
        vImag_orig[i] = vImag[i];
      }

      // ===== LCD =====
      digitalWrite(SS_LCD, LOW);

      for (int i = 0; i < N; i++) {
        SPI.transfer(original[i]);
      }

      int16_t dc = (int16_t)(vReal_orig[0] * SCALE);
      SPI.transfer(highByte(dc));
      SPI.transfer(lowByte(dc));

      for (int i = 1; i < N/2; i++) {
        int16_t real = (int16_t)(vReal_orig[i] * SCALE);
        int16_t imag = (int16_t)(vImag_orig[i] * SCALE);

        SPI.transfer(highByte(real));
        SPI.transfer(lowByte(real));
        SPI.transfer(highByte(imag));
        SPI.transfer(lowByte(imag));
      }

      int16_t nyq = (int16_t)(vReal_orig[N/2] * SCALE);
      SPI.transfer(highByte(nyq));
      SPI.transfer(lowByte(nyq));

      digitalWrite(SS_LCD, HIGH);

      // ===== AUDIO =====
      for (int i = 1; i < N/2; i++) {
        float gain = 1.0 / (1.0 + 0.5 * i);
        vReal[i] *= gain;
        vImag[i] *= gain;
      }

      for (int i = 1; i < N/2; i++) {
        int16_t real = (int16_t)(vReal[i] * SCALE);
        int16_t imag = (int16_t)(vImag[i] * SCALE);

        Serial1.write(highByte(real));
        Serial1.write(lowByte(real));
        Serial1.write(highByte(imag));
        Serial1.write(lowByte(imag));
      }

      while (!Serial1.available());
      Serial1.read();
    }
  }
}