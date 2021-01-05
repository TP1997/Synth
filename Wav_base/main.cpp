#include <cmath>
#include <fstream>
#include <iostream>
using namespace std;

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

template<typename T1, typename T2>
void write_wavfile3(const char *fname, const WaveFileHeader& wav_header, const T1* samples1, const T1* samples2, const int N){
	
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
	for(int n=0; n<N; n++){
		write_word( f, (T2)samples1[n], 2 );
		write_word( f, (T2)samples2[n], 2 );
	}
	
	f.close();
}

void float_to_audio(float *rawData, int8_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		float s = (rawData[i]+1)*INT8_MAX;
		s = std::clamp(s, float(.0), float(INT8_MAX*2));
		audioData[i] = int8_t(s);
	}
}

void float_to_audio(float *rawData, int16_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		float s = rawData[i]*INT16_MAX;
		s = std::clamp(s, float(INT32_MIN), float(INT16_MAX));
		audioData[i] = int16_t(s);
	}
}

void float_to_audio(float *rawData, int32_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		double s = double(rawData[i]*2147483647.0);
		s = std::clamp(s, -2147483647.0, 2147483647.0);
		audioData[i] = int32_t(s);
	}
}

int main(){
		
	// Write the audio samples
	// (We'll generate a single C4 note with a sine wave, fading from left to right)
	constexpr double two_pi = 6.283185307179586476925286766559;
	constexpr double max_amplitude = 32760;  // "volume"
	
	float sr        = 44100;    // samples per second
	float frequency = 261.626;  // middle C
	float seconds   = 2.5;      // time
	int channels = 2;
	
	int N = sr * seconds;  // total number of samples
	float *samples1 = new float[N]; 
	float *samples2 = new float[N];
	for (int n = 0; n < N; n+=1){
		float amplitude = (float)n / N * max_amplitude;
		float value     = sin( (two_pi * n * frequency) / sr );
		
		samples1[n] = amplitude  * value;//(int16_t)(amplitude  * value);
		samples2[n] = (max_amplitude - amplitude) * value;//(int16_t)((max_amplitude - amplitude) * value);
	}
		
	// Write wav-file
	WaveFileHeader header(N, channels, sr, sizeof(int16_t));
	write_wavfile3<float, int16_t>("test4.wav", header, samples1, samples2, N);
	
	cout << "Done!" << endl;
	return 0;
} 
