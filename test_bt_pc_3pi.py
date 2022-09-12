from serial import Serial
import json
import time

ser = Serial('COM3', 9600)  # open serial port

if not ser.is_open:
    ser.open()
    
print(ser.name)         # check which port was really used
ser.write(b'1')     # write a string
# print(b'-1')
# print(str.encode('-1'))
for _ in range(20):
    text = ser.readline()
    print(text.decode("utf-8"))
