import pyaudio
import wave
import sys

if __name__ == "__main__":
    channel = 2
    rate = 44100
    bit_depth = pyaudio.paInt16 # 16-bit resolution
    chunk = 4096 # 2^12 samples for buffer
    record_secs = 10
    filename = "audio/test.wav"

    # create pyaudio instantiation
    p = pyaudio.PyAudio()

    """
    print("Device information:")
    for index in range(p.get_device_count()):
        print("\t" + p.get_device_info_by_index(index).get("name"))
    """

    # create pyaudio stream
    stream = p.open(
        format=bit_depth,
        channels=channel,
        rate=rate,
        frames_per_buffer=chunk,
        input=True)

    print("recording...")

    print("="*20)
    print("name: {}\nchannel: {}\nsample rate: {}Hz\nsample bit depth: {}-bit".\
        format(filename, channel, rate, 
            p.get_sample_size(bit_depth) * 8))
    print("="*20)

    frames = []
    for i in range(0, int((rate / chunk) * record_secs)):
        data = stream.read(chunk)
        frames.append(data)
    
    print("finished recording")

    # stop the stream, close it, 
    # and terminate the pyaudio instantiation
    stream.stop_stream()
    stream.close()
    p.terminate()

    # save the audio frames as .wav file
    wf = wave.open(filename, 'wb')
    # wave.setparams(nchannels, sampwidth, framerate, nframes, comptype, compname)
    wf.setparams((channel, p.get_sample_size(bit_depth), 44100, 0, 'NONE', 'not compressed'))
    wf.writeframes(b''.join(frames))
    wf.close()

    sys.exit()
    