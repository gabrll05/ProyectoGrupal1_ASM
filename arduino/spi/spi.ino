#include <SPI.h>
#include <arduinoFFT.h>
#include <math.h>

#define N 32
#define CS 10
#define SAMPLING_FREQUENCY 1000

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, SAMPLING_FREQUENCY, false);

byte index = 0;

void setup() {
  Serial.begin(115200);

  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  SPI.begin();
}

void loop() {
  if (Serial.available()) {

    vReal[index] = Serial.read();
    vImag[index] = 0;
    index++;

    if (index >= N) {
      index = 0;

      FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.compute(FFT_FORWARD);
      FFT.complexToMagnitude();

      digitalWrite(CS, LOW);
      delayMicroseconds(10);

      for (int i = 0; i < N / 2; i++) {

        double mag = vReal[i];
        if (mag < 1) mag = 1;

        byte val = (byte)(log(mag) * 25);  
        if (val > 255) val = 255;

        SPI.transfer(val);
      }

      delayMicroseconds(10);
      digitalWrite(CS, HIGH);
    }
  }
}