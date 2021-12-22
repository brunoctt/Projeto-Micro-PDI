import serial

ser = serial.Serial('COM7', 9600)  # open serial port

if not ser.is_open:
    ser.open()
    
print(ser.name)         # check which port was really used
ser.write(b'-1')     # write a string
# print(b'-1')
# print(str.encode('-1'))
text = ser.readline()
print(text)
print(text.decode("utf-8"))
# print(ser.readline())
# print(ser.readline())
# # print(str(ser.read(1)))
