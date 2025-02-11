import serial 
import numpy as np
import matplotlib.pyplot as plt
import time, os
import datetime
import argparse
import logging


###
### hyperparameters
###

tty = '/dev/ttyACM0'
baudrate=115200
adc_samples = 1024  ##this should match the values at the cpp code
raw_folder = 'raw_data'

parse_data = False
parse_folder = 'csv_data'

##BTW with the 2kHz sampling rate each day we generate
##  2000*4*60*60*24/2**20 = 659MB of data!!

parser = argparse.ArgumentParser(
    description="Get data from the raspberry pico ADCs")

parser.add_argument("-t", "--time", dest="rec_time", default=24*60, type=float,
                help="time to record in minutes, default is 24rs")



def quick_parse_data(raw_filename, first_samples=2*(1024*4+4)):
    """
    This is a dirty approach, only search for the first 0xaabbccdd
    and then assume that the raw data is in order
    raw_fd: raw_data file descriptor

    The raw data is composed by frames where each frane started with 4 bytes
    0xaabbccdd and after that there are 1024*4 interleaved adc samples where
    one of the adc samples is from the temperature sensor and can be used to
    detect errors
    """
    fr = open(raw_filename, 'rb')
    peek = np.frombuffer(fr.read(first_samples*4), '>u4')
    index = np.where(peek == 0xaabbccdd)[0][0]
    size = os.path.getsize(raw_filename)
    fr.seek(index*4)
    frame_size = 1024*4+4   ##in bytes
    ##CHECK!!! seems to be buggy
    n_frames = (size-index*4)//frame_size
    raw_data = fr.read(n_frames*frame_size)
    data = np.frombuffer(raw_data, '>u1')
    data = data.reshape((-1, frame_size))
    adc0 = data[:,4:][:,::4].flatten()
    adc1 = data[:,4:][:,1::4].flatten()
    adc2 = data[:,4:][:,2::4].flatten()
    adc3 = data[:,4:][:,3::4].flatten()
    return adc0, adc1, adc2, adc3
    
    


if __name__ == '__main__':
    args = parser.parse_args()
    rec_time = args.rec_time*60
    print("rec time %f secs"%rec_time)
    ###The idea is to run the script at the begining of each day with a cronjob
    samples = adc_samples*4+4   ##We have 4ADC data (1 is to check correctness)
                                ##and the word 0xaabbccdd is at the begining of 
                                ##each data frame

    ser = serial.Serial(tty, baudrate)
    ##to start the transmission from the pico you have to write whatever
    os.makedirs(raw_folder, exist_ok=True)
    start_stamp = time.time()
    start_date = datetime.datetime.fromtimestamp(start_stamp)
    end_stamp = start_stamp+rec_time
    raw_filename = os.path.join(raw_folder, start_date.strftime("%m-%d-%Y_%H:%M:%S"))
    f = open(raw_filename, 'wb')
    ser.write('a'.encode())
    while(1):
        curr_time = time.time()
        if(curr_time>end_stamp):
            print('out of the while loop')
            break
        raw_data = ser.read(samples)
        f.write(raw_data)
    ser.close()
    f.close()
