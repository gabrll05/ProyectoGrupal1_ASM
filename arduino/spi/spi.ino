#include <SPI.h>
#include <arduinoFFT.h>
#include <math.h>

#define N 32
#define CS_LCD   10
#define CS_AUDIO 9

double vReal[N];
double vImag[N];

ArduinoFFT<double> FFT(vReal, vImag, N, 1000);

byte index = 0;

void setup() {
  Serial.begin(115200);

  pinMode(CS_LCD, OUTPUT);
  pinMode(CS_AUDIO, OUTPUT);

  digitalWrite(CS_LCD, HIGH);
  digitalWrite(CS_AUDIO, HIGH);

  SPI.begin();
}

void loop() {
  if (Serial.available()) {

    vReal[index] = Serial.read();
    vImag[index] = 0;
    index++;

    if (index >= N) {
      index = 0;

      FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
      FFT.compute(FFTDirection::Forward);
      FFT.complexToMagnitude();


      digitalWrite(CS_LCD, LOW);

      for (int i = 0; i < N/2; i++) {
        double mag = vReal[i];
        if (mag < 1) mag = 1;

        byte val = (byte)(log(mag) * 25);
        if (val > 255) val = 255;

        SPI.transfer(val);
      }

      digitalWrite(CS_LCD, HIGH);

      int top[3] = {1,2,3};
      byte mag[3] = {0,0,0};

      for (int i = 1; i < N/2; i++) {
        for (int j = 0; j < 3; j++) {
          if (vReal[i] > vReal[top[j]]) {
            for (int k = 2; k > j; k--) {
              top[k] = top[k-1];
              mag[k] = mag[k-1];
            }
            top[j] = i;
            mag[j] = min((int)vReal[i], 255);
            break;
          }
        }
      }

      digitalWrite(CS_AUDIO, LOW);

      for (int i = 0; i < 3; i++) {
        SPI.transfer((byte)top[i]);
        SPI.transfer(mag[i]);
      }

      digitalWrite(CS_AUDIO, HIGH);
      Serial.write('K');
    }
  }
}
