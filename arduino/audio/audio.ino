#define N 32
#define AUDIO_PIN D2

double vReal[N];
double vImag[N];

int idx = 1;

void setup() {
  Serial.begin(115200);

  analogWriteRange(255);
  analogWriteFreq(20000); 
}

void loop() {

  if (Serial.available() >= 4) {

    int16_t real = (Serial.read() << 8) | Serial.read();
    int16_t imag = (Serial.read() << 8) | Serial.read();

    vReal[idx] = real;
    vImag[idx] = imag;

    idx++;

    if (idx >= N/2) {
      idx = 1;

      // reconstrucción simétrica
      vReal[0] = 0; vImag[0] = 0;
      vReal[N/2] = 0; vImag[N/2] = 0;

      for (int i = 1; i < N/2; i++) {
        vReal[N-i] = vReal[i];
        vImag[N-i] = -vImag[i];
      }

      // IFFT
      for (int k = 0; k < N; k++) {
        double sum = 0;

        for (int n = 0; n < N; n++) {
          double angle = 2 * PI * k * n / N;
          sum += vReal[n]*cos(angle) - vImag[n]*sin(angle);
        }

        int out = (int)(sum / N) + 127;
        out = constrain(out, 0, 255);

        analogWrite(AUDIO_PIN, out);
        delayMicroseconds(125);
      }

      Serial.write('K');
    }
  }
}