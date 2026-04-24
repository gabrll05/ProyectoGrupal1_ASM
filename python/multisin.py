import serial
import time
import numpy as np

PORT = '/dev/ttyACM0'
BAUD = 115200
SR = 8000
BLOCK = 32


F1 = 250
F2 = 750
F3 = 2250

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

def gen_multi(duration):
    t = np.arange(0, duration, 1/SR)

    wave = (
        np.sin(2 * np.pi * F1 * t) +
        np.sin(2 * np.pi * F2 * t) +
        np.sin(2 * np.pi * F3 * t)
    ) / 3.0

    return (wave * 40 + 127).astype(np.uint8)

audio = gen_multi(5)  

i = 0

while True:
    block = audio[i:i+BLOCK]

    if len(block) < BLOCK:
        i = 0
        continue

    ser.write(block.tobytes())

    i += BLOCK
    time.sleep(BLOCK / SR)