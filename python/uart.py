import serial
import time

ser = serial.Serial('/dev/serial/by-id/usb-Arduino_LLC_Arduino_Leonardo-if00', 115200)

time.sleep(2)  # esperar reset del Leonardo

while True:
    msg = input("Enviar: ")
    ser.write((msg + '\n').encode())