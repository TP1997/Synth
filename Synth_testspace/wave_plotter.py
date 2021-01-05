import numpy as np 
from scipy.io.wavfile import read
import librosa as lr
import librosa.display as display

#%%
'''
auto oscillator_sine(float &phase, const float fq, const float sr){
	
	phase += 2*pi*fq/sr;
	while(phase >= 2*pi)
		phase -= 2*pi;
	while(phase < 0)
		phase += 2*pi;
		
	return sin(phase);

auto oscillator_square(float &phase, const float fq, const float sr){
	
	phase += fq/sr;
	while(phase > 1.0f)
		phase -= 1.0f;
	while(phase < 0.0f)
		phase += 1.0f;
	if(phase <= 0.5f)
		return -1.0f;
	else
		return 1.0f;
}

auto oscillator_saw(float &phase, const float fq, const float sr){

	phase += fq/sr;
	while(phase > 1.0f)
		phase -= 1.0f;
	while(phase < 0.0f)
		phase += 1.0f;
		
	return (phase * 2.0f) - 1.0f;
}

auto oscillator_triangle(float &phase, const float fq, const float sr){

	phase += fq/sr;
	while(phase > 1.0f)
		phase -= 1.0f;
	while(phase < 0.0f)
		phase += 1.0f;
	
	float ret;
	if(phase <= 0.5f)
		ret = phase*2.0f;
	else
		ret = (1.0f - phase) * 2.0f;
	
	return (ret * 2.0f) - 1.0f;

'''
def oscillator_sine(phase, fq, sr):
    phase += 2*np.pi*fq/sr
    while phase >= 2*np.pi:
        phase -= 2*np.pi
    while phase < 0:
        phase += 2*np.pi
        
    return np.sin(phase), phase

def oscillator_square(phase, fq, sr):
    phase += fq/sr
    while phase > 1.0:
        phase -= 1.0
    while phase < 0.0:
        phase += 1.0
    if phase <= 0.5:
        return -1.0, phase
    else:
        return 1.0, phase

def oscillator_saw(phase, fq, sr):
    phase += fq/sr
    while phase > 1.0:
        phase -= 1.0
    while phase < 0.0:
        phase += 1.0
        
    return (phase * 2.0) - 1.0, phase

def oscillator_triangle(phase, fq, sr):
    phase += fq/sr
    while phase > 1.0:
        phase -= 1.0
    while phase < 0.0:
        phase += 1.0
        
    ret = 0
    if phase <= 0.5:
        ret = phase * 2.0
    else:
        ret = (1.0 - phase) * 2.0
        
    return (ret * 2.0) - 1.0, phase

def calc_frequency(octave, note):
    
    return 440.0 * np.power(2.0, ((octave - 4.0) * 12.0 + note)/12.0)

#%% Generate
sr = 44100
seconds = 4
N = sr * seconds
freq = calc_frequency(3,3);
phase = 0;
rawData = []

for n in range(0, N):
    sample, phase = oscillator_triangle(phase, freq, sr)
    rawData.append(sample)
    
rawData = np.array(rawData)

#%% Plot
import matplotlib.pyplot as plt

x = np.linspace(0, 4, N)
plt.plot(x[0:1000], rawData[0:1000], '-')






























































