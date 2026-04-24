import numpy as np
import matplotlib.pyplot as plt

SR = 8000
N = 32

def gen_tone(freq, duration):
    t = np.arange(0, duration, 1/SR)
    return np.sin(2 * np.pi * freq * t)

def gen_multi(duration):
    t = np.arange(0, duration, 1/SR)
    return (
        np.sin(2 * np.pi * 250 * t) +
        np.sin(2 * np.pi * 750 * t) +
        np.sin(2 * np.pi * 1250 * t)
    ) / 3.0

def gen_tone_silence(freq):
    tone = gen_tone(freq, 0.1)
    silence = np.zeros(int(0.1 * SR))
    return np.concatenate([tone, silence])

signals = [
    ("Seno 500Hz", gen_tone(500, 0.2)),
    ("Multiespectral", gen_multi(0.2)),
    ("Tono + Silencio", gen_tone_silence(200))
]

plt.figure(figsize=(14,10))

for idx, (name, signal) in enumerate(signals):

    signal = signal[:N]

    fft_vals = np.fft.fft(signal)
    ifft_vals = np.fft.ifft(fft_vals).real

    base = idx * 3

    plt.subplot(3,3,base+1)
    plt.title(f"{name}\nOriginal")
    plt.plot(signal)

    plt.subplot(3,3,base+2)
    plt.title("FFT")
    plt.stem(np.abs(fft_vals), use_line_collection=True)

    plt.subplot(3,3,base+3)
    plt.title("IFFT")
    plt.plot(ifft_vals)

plt.tight_layout()
plt.show()