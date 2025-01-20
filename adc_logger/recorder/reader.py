import serial 
import numpy as np
import matplotlib.pyplot as plt
import time

##note!! by default the buffer by default is 4096bytes!
##each time there is data available the raspi will trow 3*samples and each sample starts with 
tty = '/dev/ttyACM0'
baudrate=115200
adc_samples = 1024#128
samples = adc_samples*4+4
iters = 32#1024

raw_data = b""

ser = serial.Serial(tty, baudrate)
ser.write('a'.encode())
for i in range(iters):
    print(i)
    raw_data += ser.read(samples)

ser.close()

#find the headers 
cast_data = np.frombuffer(raw_data, '>u4')
indices = np.where(cast_data==0xaabbccdd)[0]

adc0 = []
adc1 = []
adc2 = []
adc3 = []
for i in range(len(indices)-1):
    window = cast_data[indices[i]:indices[i+1]].view('>u1')[4:]
    adc0+=window[::4].tolist()
    adc1+=window[1::4].tolist()
    adc2+=window[2::4].tolist()
    adc3+=window[3::4].tolist()

adc0 = np.array(adc0)
adc1 = np.array(adc1)
adc2 = np.array(adc2)
adc3 = np.array(adc3)
