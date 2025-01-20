import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import serial 
import time


###
### hyperparameters
###

tty = '/dev/ttyACM0'
baudrate=115200
adc_samples = 1024  ##this should match the values at the cpp code
fs = 2000

samples = adc_samples*4+4

###
###
###

fig, axes = plt.subplots(2,2)
lines = []
for ax in axes.flatten():
    ax.set_ylim(0, 256)
    ax.grid()
    ax.set_xlim(0, adc_samples*1/fs)
    line, = ax.plot([],[], animated=True)
    lines.append(line)



ser = serial.Serial(tty, baudrate)
x_values = np.arange(adc_samples)*1/fs

def get_data():
    raw_data = b""
    dat = ser.read(samples)
    raw_data+= dat
    dat = ser.read(samples)
    raw_data+= dat
    ##just in case we are in the middle ot two frames we take 2 and just keep one
    ##of the frames
    cast_data = np.frombuffer(raw_data, '>u4')
    indices = np.where(cast_data == 0xaabbccdd)[0]
    if(len(indices)==0):
        return np.zeros((4, adc_samples))
    window = cast_data[indices[0]:indices[0]+1025].view('>u1')[4:]
    return window.reshape((-1,4)).T


def plot_data(i):
    data = get_data()
    for i, line in enumerate(lines):
        line.set_data(x_values, data[i,:])
    return lines


ani = FuncAnimation(fig, plot_data, blit=True)
plt.show()








