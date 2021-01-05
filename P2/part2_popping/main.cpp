#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

constexpr double pi = 3.14159265359;

namespace little_endian_io{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (;size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

struct WaveFileHeader{
	public:
	const char* m_ChunkID 			= "RIFF";
	int32_t m_ChunkSize;
	const char* m_Format 			= "WAVE";
	
	const char* m_Subchunk1ID 		= "fmt ";
	const int32_t m_Subchunk1Size 	= 16;
	const int16_t m_AudioFormat		= 1;
	int16_t m_NumChannels;
	int32_t m_SampleRate;
	int32_t m_ByteRate;
	int16_t m_BlockAlign;
	int16_t m_BitsPerSample;
	
	const char* m_Subchunk2ID 		= "data";
	int32_t m_Subchunk2Size;
	
	WaveFileHeader(int N, int16_t NumChannels, int32_t SampleRate, size_t sample_size){
		m_NumChannels 	= NumChannels;
		m_SampleRate  	= SampleRate;
		m_BitsPerSample = sample_size * 8;//sizeof(T) * 8;
		m_ByteRate		= SampleRate * NumChannels * m_BitsPerSample / 8;
		m_BlockAlign	= NumChannels * m_BitsPerSample / 8;
		m_Subchunk2Size	= N * NumChannels * m_BitsPerSample / 8;
		m_ChunkSize		= 36 + m_Subchunk2Size;
	}
};

void float_to_audio(const float *rawData, int8_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		float s = (rawData[i]+1)*INT8_MAX;
		s = std::clamp(s, float(.0), float(INT8_MAX*2));
		audioData[i] = int8_t(s);
	}
}

void float_to_audio(const float *rawData, int16_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		float s = rawData[i]*INT16_MAX;
		s = std::clamp(s, float(INT32_MIN), float(INT16_MAX));
		audioData[i] = int16_t(s);
	}
}

void float_to_audio(const float *rawData, int32_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		double s = double(rawData[i]*2147483647.0);
		s = std::clamp(s, -2147483647.0, 2147483647.0);
		audioData[i] = int32_t(s);
	}
}

// Calculate frequency of given note
float calc_frequency(float octave, float note){
/*
	0  = A
	1  = A#
	2  = B
	3  = C
	4  = C#
	5  = D
	6  = D#
	7  = E
	8  = F
	9  = F#
	10 = G
	11 = G# 
*/

	return float(440*pow(2.0, double(((octave-4)*12+note)/12.0)));

}

//specify sample bit count as the template parameter
//can be uint8, int16 or int32
template<typename T1, typename T2>
void write_wavfile(const char *fname, const WaveFileHeader& wav_header, const T1* raw_samples, const int N){
	
	std::ofstream f(fname, std::ios::binary);
	f << wav_header.m_ChunkID;
	write_word(f, wav_header.m_ChunkSize);
	f << wav_header.m_Format << wav_header.m_Subchunk1ID;
	
	write_word(f, wav_header.m_Subchunk1Size);
	write_word(f, wav_header.m_AudioFormat);
	write_word(f, wav_header.m_NumChannels);
	write_word(f, wav_header.m_SampleRate);
	write_word(f, wav_header.m_ByteRate);
	write_word(f, wav_header.m_BlockAlign);
	write_word(f, wav_header.m_BitsPerSample);
	
	f << wav_header.m_Subchunk2ID;
	write_word(f, wav_header.m_Subchunk2Size);
	
	// Fill the data
	T2 *audio_samples = new T2[N];
	float_to_audio(raw_samples, audio_samples, N);
	
	for(int n=0; n<N; n++){
		write_word( f, audio_samples[n], 2 );
		//write_word( f, (T2)samples2[n], 2 );
	}
	
	f.close();
	delete[] audio_samples;
}

auto oscillator_sine(float &phase, const float fq, const float sr){
	
	phase += 2*pi*fq/sr;
	while(phase >= 2*pi)
		phase -= 2*pi;
	while(phase < 0)
		phase += 2*pi;
		
	return sin(phase);
}


int main(){
	
	int sr = 44100;
	int channels = 1;
	float seconds = 2.;
	int N = sr * channels * seconds;	

	float *rawData = new float[N];
	float freq = calc_frequency(3,3);
	float phase = 0;
	//make a discontinuitous (popping) sine wave
	for(int n=0; n<N; n++){
		if(n>N/2){
			freq = calc_frequency(3,4);
		}
		
		rawData[n] = oscillator_sine(phase, freq, sr);	
		//rawData[n] = sin(n * 2 * pi * freq / sr);
	}
		
	WaveFileHeader header(N, channels, sr, sizeof(int16_t));
	write_wavfile<float, int16_t>("corrected_sine2.wav", header, rawData, N);	  
	std::cout << "Done!\n" << std::endl;
	
	delete[] rawData;
    return 0;
}












