#include <arduinoFFT.h>

#define N 32

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 8000);

int index = 0;

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
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

     
    for (int i = 1; i < N/2; i++) {

  float gain = 1.0 / (1.0 + 0.5 * i); 

  vReal[i] *= gain;
  vImag[i] *= gain;

  int16_t real = (int16_t)(vReal[i] * 8);
  int16_t imag = (int16_t)(vImag[i] * 8);

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