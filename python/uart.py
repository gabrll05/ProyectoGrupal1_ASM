import serial
import time
import numpy as np
import librosa

PORT = '/dev/ttyACM0'
BAUD = 115200

ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)

audio_raw, _ = librosa.load("audio/song1.wav", sr=8000, mono=True)

audio = audio_raw / np.max(np.abs(audio_raw))
audio = (audio * 127 + 127).astype(np.uint8)

i = 0

while True:
    ser.write(audio[i:i+32].tobytes())
    i += 32

    if i >= len(audio):
        i = 0

    time.sleep(0.002)