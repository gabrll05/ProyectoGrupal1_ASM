#include <SPI.h>
#include <LiquidCrystal.h>
#include <math.h>

LiquidCrystal lcd(2,3,4,5,6,7);

#define N 32
#define SCALE 8

volatile byte data;
volatile bool flag = false;

byte original[N];
double vReal[N];
double vImag[N];
double reconstructed[N];

int idx = 0;
int stage = 0;

int bytePhase = 0;
int bin_i = 0;
int16_t temp = 0;

// ===== caracteres personalizados =====
byte levels[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,31},
  {0,0,0,0,0,0,31,31},
  {0,0,0,0,0,31,31,31},
  {0,0,0,0,31,31,31,31},
  {0,0,0,31,31,31,31,31},
  {0,0,31,31,31,31,31,31},
  {0,31,31,31,31,31,31,31}
};

void plotWave() {

  double minV = 9999;
  double maxV = -9999;

  for (int i = 0; i < N; i++) {
    if (reconstructed[i] < minV) minV = reconstructed[i];
    if (reconstructed[i] > maxV) maxV = reconstructed[i];
  }

  lcd.clear();

  for (int x = 0; x < 16; x++) {

    int idx = x * 2;
    double val = reconstructed[idx];

    int level = 0;

    if (maxV != minV) {
      level = (int)((val - minV) * 7 / (maxV - minV));
    }

    if (level < 0) level = 0;
    if (level > 7) level = 7;

    lcd.setCursor(x,1);
    lcd.write(byte(level));
  }

  delay(1500);
}

void setup() {
  lcd.begin(16,2);

  for (int i = 0; i < 8; i++) {
    lcd.createChar(i, levels[i]);
  }

  pinMode(MISO, OUTPUT);
  pinMode(10, INPUT_PULLUP);

  SPCR = _BV(SPE);
  SPI.attachInterrupt();

  lcd.print("RUN");
}

ISR(SPI_STC_vect) {
  data = SPDR;
  flag = true;
}

void loop() {

  if (!flag) return;
  flag = false;

  if (stage == 0) {
    original[idx++] = data;

    if (idx >= N) {
      idx = 0;
      stage = 1;
      bytePhase = 0;
      bin_i = 0;

      for (int i = 0; i < N; i++) {
        vReal[i] = 0;
        vImag[i] = 0;
      }
    }
    return;
  }

  if (bin_i == 0) {
    if (bytePhase == 0) {
      temp = (int16_t)data << 8;
      bytePhase = 1;
      return;
    } else {
      temp |= data;
      vReal[0] = temp;
      vImag[0] = 0;
      bytePhase = 0;
      bin_i = 1;
      return;
    }
  }

  if (bin_i > 0 && bin_i < N/2) {

    static bool imagPart = false;

    if (bytePhase == 0) {
      temp = (int16_t)data << 8;
      bytePhase = 1;
      return;
    } else {
      temp |= data;

      if (!imagPart) {
        vReal[bin_i] = temp;
        imagPart = true;
      } else {
        vImag[bin_i] = temp;
        imagPart = false;
        bin_i++;
      }

      bytePhase = 0;
      return;
    }
  }

  if (bin_i == N/2) {
    if (bytePhase == 0) {
      temp = (int16_t)data << 8;
      bytePhase = 1;
      return;
    } else {
      temp |= data;
      vReal[N/2] = temp;
      vImag[N/2] = 0;

      for (int i = 1; i < N/2; i++) {
        vReal[N-i] = vReal[i];
        vImag[N-i] = -vImag[i];
      }

      for (int k = 0; k < N; k++) {
        double sum = 0;

        for (int n = 0; n < N; n++) {
          double ang = 2 * PI * k * n / N;
          sum += vReal[n]*cos(ang) - vImag[n]*sin(ang);
        }

        reconstructed[k] = sum / (N * SCALE);
      }

      double e0 = 0;
      double e1 = 0;

      for (int i = 0; i < N; i++) {
        double o = original[i] - 127;
        double r = reconstructed[i];

        e0 += o * o;
        e1 += r * r;
      }

      double gain = (e1 > 0) ? sqrt(e0 / e1) : 1.0;

      for (int i = 0; i < N; i++) {
        reconstructed[i] *= gain;
      }

      static double mse_avg = 0;
      static double ep_avg = 0;
      static double ser_avg = 0;
      static int count = 0;

      double mse = 0;
      double e0_final = 0;

      for (int i = 0; i < N; i++) {

        double o = original[i] - 127;
        double r = reconstructed[i];

        double err = o - r;

        mse += err * err;
        e0_final += o * o;
      }

      mse /= N;

      double EP = (e0_final == 0) ? 0 : (mse / e0_final) * 100.0;
      double SER = (mse > 0) ? 10 * log10(e0_final / mse) : 0;

      mse_avg += mse;
      ep_avg += 100 - EP;
      ser_avg += SER;
      count++;

      if (count >= 10) {

        lcd.clear();

        lcd.setCursor(0,0);
        lcd.print("MSE:");
        lcd.print(mse_avg / count,0);

        lcd.setCursor(0,1);
        lcd.print("EP:");
        lcd.print(ep_avg / count,1);
        lcd.print("%");

        delay(1200);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SER:");
        lcd.print(ser_avg / count,1);

        delay(1200);

        plotWave();

        mse_avg = 0;
        ep_avg = 0;
        ser_avg = 0;
        count = 0;
      }

      stage = 0;
      bin_i = 0;
      bytePhase = 0;
    }
  }
}