import serial
import time
import numpy as np
import librosa

PORT = '/dev/ttyACM0'
BAUD = 115200
BLOCK = 32

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

audio_raw, _ = librosa.load("audio/song1.wav", sr=8000, mono=True)

audio = audio_raw / np.max(np.abs(audio_raw))
audio = (audio * 80 + 127).astype(np.uint8)

i = 0

while True:
    block = audio[i:i+BLOCK]

    if len(block) < BLOCK:
        i = 0
        continue
    
    ser.write(block.tobytes())

    while True:
        if ser.in_waiting:
            if ser.read() == b'K':
                break

    i += BLOCK