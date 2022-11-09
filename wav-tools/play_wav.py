import pyaudio
import wave
import sys
import time

CHUNK = 44100

if __name__ == "__main__":
    if len(sys.argv) == 2:
        filename = sys.argv[1]
    else:
        filename = "audio/init.wav"
    wf = wave.open(filename, 'rb')

    # create pyaudio instantiation
    p = pyaudio.PyAudio()

    # create pyaudio stream
    stream = p.open(
        format=p.get_format_from_width(wf.getsampwidth()),
        channels=wf.getnchannels(),
        rate=wf.getframerate(),
        output=True)

    print("playing...")

    print("="*20)
    print("name: {}\nchannel: {}\nsample rate: {}Hz\nsample bit depth: {}-bit".\
        format(filename,
            wf.getnchannels(), 
            wf.getframerate(), 
            wf.getsampwidth() * 8))
    print("="*20)

    data = wf.readframes(CHUNK)
    while data != b'':
        stream.write(data)
        data = wf.readframes(CHUNK)
    
    print("End!")
    
    # stop the stream, close it, 
    # and terminate the pyaudio instantiation
    stream.stop_stream()
    stream.close()
    p.terminate()

    wf.close()
    sys.exit()