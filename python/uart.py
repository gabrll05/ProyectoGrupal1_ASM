import serial
import time
import numpy as np

PORT = '/dev/ttyACM0'
BAUD = 115200
SR = 8000
BLOCK = 32
FREQ = 2000

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

def gen_tone(freq, duration):
    t = np.arange(0, duration, 1/SR)
    wave = np.sin(2 * np.pi * freq * t)
    return (wave * 40 + 127).astype(np.uint8)

def gen_silence(duration):
    samples = int(duration * SR)
    return np.ones(samples, dtype=np.uint8) * 127


audio = np.concatenate([
    gen_tone(FREQ, 0.1),
    gen_silence(0.1)
])

i = 0

while True:
    block = audio[i:i+BLOCK]

    if len(block) < BLOCK:
        i = 0
        continue

    ser.write(block.tobytes())

    i += BLOCK
    time.sleep(BLOCK / SR)