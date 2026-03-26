import numpy as np
import matplotlib.pyplot as plt

# Parámetros
SR = 80000        # sample rate (frecuencia de muestreo)
T = 5             # duración en segundos
F = 5             # frecuencia de la señal

def generate_sin_signal(freq, sample_rate, duration):
    t = np.linspace(0, duration, sample_rate * duration, endpoint=False)
    y = np.sin(2 * np.pi * freq * t)
    return t, y

# Generar señal
t, y = generate_sin_signal(F, SR, T)

# ----------- FFT -----------
N = len(y)  # número de muestras

fft = np.fft.fft(y)              # Transformada
frequencies = np.fft.fftfreq(N, 1/SR)  # eje de frecuencias
magnitude = np.abs(fft)          # magnitud

# ----------- Gráficas -----------

# Señal en el tiempo
plt.figure(figsize=(12,5))
plt.subplot(1,2,1)
plt.plot(t, y)
plt.title("Señal en el tiempo")
plt.xlabel("Tiempo (s)")
plt.ylabel("Amplitud")

# Espectro (solo parte positiva)
plt.xlim(0, 20)
plt.subplot(1,2,2)
plt.plot(frequencies[:N//2], magnitude[:N//2])
plt.title("Espectro de frecuencia")
plt.xlabel("Frecuencia (Hz)")
plt.ylabel("Magnitud")

plt.tight_layout()
plt.show()