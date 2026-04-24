import serial
import time
import numpy as np

PORT = '/dev/ttyACM0'
BAUD = 115200
SR = 8000
BLOCK = 32
FREQ = 500  # frecuencia limpia (bin exacto)

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

def gen_tone(freq, duration):
    t = np.arange(0, duration, 1/SR)
    wave = np.sin(2 * np.pi * freq * t)
    return (wave * 40 + 127).astype(np.uint8)

audio = gen_tone(FREQ, 5)  # 5 segundos continuo

i = 0

while True:
    block = audio[i:i+BLOCK]

    if len(block) < BLOCK:
        i = 0
        continue

    ser.write(block.tobytes())

    i += BLOCK
    time.sleep(BLOCK / SR)