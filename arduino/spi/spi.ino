#include <arduinoFFT.h>

#define N 32

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 8000);

int index = 0;

void setup() {
  Serial.begin(115200);   // PC
  Serial1.begin(115200);  // ESP
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

        int16_t mag = (int16_t)vReal[i];
        int16_t phase = (int16_t)vImag[i];

        Serial1.write(highByte(mag));
        Serial1.write(lowByte(mag));
        Serial1.write(highByte(phase));
        Serial1.write(lowByte(phase));
      }

      // esperar ACK del ESP
      while (!Serial1.available());
      if (Serial1.read() == 'K') {
        // ok
      }
    }
  }
}