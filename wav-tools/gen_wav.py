import struct
import numpy as np
from scipy import signal as sg
import wave 

if __name__ == "__main__":
    # Sampling information
    fs = 44100    # Sample rate
    Ts = 1.0 / fs # Sample period
    t = 1.0 # duration
    N = fs * t

    # Signal information
    f = 1000    # signal frequency
    channel = 2 # stereo
    amplitude = 2000

    ####### sine wave ###########
    x = np.arange(0, t, Ts)
    # 這裡需要放大是因為 wf.writeframes() 是存binary的
    # 假設沒放大那麼振幅範圍是在(-1, 1)之間,
    # 在轉型成 int() 時會全部變成0
    y = amplitude * np.sin(2 * np.pi * f * x)

    wf = wave.open("audio/gen.wav",'w')
    # wave.setparams(nchannels, sampwidth, framerate, nframes, comptype, compname)
    wf.setparams((channel, 2, fs, 0, 'NONE', 'not compressed'))

    # 2 channel: stereo

    if channel == 1:
        mono = y
        for data in mono:
            wf.writeframes(struct.pack('h', int(data)))
    elif channel == 2:
        stereo = zip(y, y)
        for left, right in stereo:
            wf.writeframes(struct.pack('h', int(left)))
            wf.writeframes(struct.pack('h', int(right)))
    else:
        print("Not support!")
    
    wf.close()
