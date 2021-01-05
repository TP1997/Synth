import numpy as np 
from scipy.io.wavfile import read
import librosa as lr
import librosa.display as display
root = '/home/tuomas/C++/Synth_stuff/P2/part2_popping/'
f1 = 'popping_sine1.wav'
f2 = 'corrected_sine1.wav'
sr = 44100

#%%
audio1 = lr.load(root + f1, sr=sr)[0][0:100]
audio2 = lr.load(root + f2, sr=sr)[0][0:100]

#%%
display.waveplot(audio1, sr=sr)

#%%
display.waveplot(audio2, sr=sr)