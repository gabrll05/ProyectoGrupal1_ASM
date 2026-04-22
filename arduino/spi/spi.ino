#include <SPI.h>
#include <arduinoFFT.h>
#include <math.h>

#define N 32
#define K 3

#define CS_AUDIO 9

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 8000);

byte index = 0;

void setup() {
  Serial.begin(115200);

  pinMode(CS_AUDIO, OUTPUT);
  digitalWrite(CS_AUDIO, HIGH);

  SPI.begin();
}

void loop() {

  if (Serial.available()) {

    vReal[index] = Serial.read() - 127;
    vImag[index] = 0;
    index++;

    if (index >= N) {
      index = 0;

      FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
      FFT.compute(FFTDirection::Forward);

      // TOP K bins (con fase)
      int top[K] = {1,2,3};
      double mag[K] = {0,0,0};

      for (int i = 1; i < N/2; i++) {

        double m = vReal[i]*vReal[i] + vImag[i]*vImag[i];

        for (int j = 0; j < K; j++) {
          if (m > mag[j]) {

            for (int k = K - 1; k > j; k--) {
              top[k] = top[k-1];
              mag[k] = mag[k-1];
            }

            top[j] = i;
            mag[j] = m;
            break;
          }
        }
      }

      // enviar por SPI
      digitalWrite(CS_AUDIO, LOW);

      for (int i = 0; i < K; i++) {

        int k = top[i];

        int16_t real = (int16_t)vReal[k];
        int16_t imag = (int16_t)vImag[k];

        SPI.transfer((byte)k);

        SPI.transfer(real >> 8);
        SPI.transfer(real & 0xFF);

        SPI.transfer(imag >> 8);
        SPI.transfer(imag & 0xFF);
      }

      digitalWrite(CS_AUDIO, HIGH);

      Serial.write('K');
    }
  }
}