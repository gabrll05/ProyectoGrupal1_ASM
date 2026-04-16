import serial
import time
import random

ser = serial.Serial('/dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00', 115200)
time.sleep(2)

while True:
    for _ in range(32):
        val = 127 + random.randint(-60, 60)  # ruido centrado en 127
        ser.write(bytes([val & 0xFF]))
        time.sleep(0.003)