#include <cstring>
#include <fstream>
#include <iostream>
#include <cmath>

namespace little_endian_io{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof(Word)){
	for (;size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF));
    return outs;
  }
}

struct WaveFileHeader{
	
	public:
	// The RIFF-chunk
	const char *m_ChunkID = "RIFF";
	uint32_t m_ChunkSize;
	const char *m_Format = "WAVE";
	//const char *m_riffChunk = "RIFF----WAVEfmt ";
	
	// The fmt-chunk
	const char *m_SubChunk1ID = "fmt ";
	const uint32_t m_SubChunk1Size = 16;
	const uint16_t m_AudioFormat = 1;
	uint16_t m_NumChannels;// = 1;
	uint32_t m_SampleRate;// = 44100;
	uint16_t m_BitsPerSample;// = 16; //16??
	uint32_t m_ByteRate;// = m_SampleRate * m_NumChannels * m_BitsPerSample / 8;
	uint16_t m_BlockAlign;// = m_NumChannels * m_BitsPerSample / 8;
	
	// The data-chunk
	const char *m_SubChunk2ID = "data";
	uint32_t m_SubChunk2Size;
	
	WaveFileHeader(int sampleRate, int numChannels, int numSamples, size_t bitsPerSample){
		m_NumChannels = numChannels;
		m_SampleRate = sampleRate;
		m_BitsPerSample = bitsPerSample;
		m_ByteRate = m_SampleRate * m_NumChannels * m_BitsPerSample / 8;
		m_BlockAlign = m_NumChannels * m_BitsPerSample / 8;
		m_SubChunk2Size = numSamples * m_NumChannels * m_BitsPerSample / 8;
		m_ChunkSize = 36 + m_SubChunk2Size;
	}
	// Data...

};

template<typename T>
void float_to_audio(T *rawData, int8_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		T s = (T)(rawData[i]+1)*INT8_MAX;
		s = std::clamp(s, (T).0, (T)INT8_MAX*2);
		audioData[i] = (int8_t)s;
	}
}

template<typename T>
void float_to_audio(T *rawData, int16_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		T s = (T)rawData[i]*INT16_MAX;
		s = std::clamp(s, (T)INT32_MIN, (T)INT16_MAX);
		audioData[i] = (int16_t)s;
	}
}

template<typename T>
void float_to_audio(T *rawData, int32_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		T s = (T)rawData[i]*2147483647.0;
		s = std::clamp(s, -2147483647.0, 2147483647.0);
		audioData[i] = (int32_t)s;
	}
}

//specify sample bit count as the template parameter
//can be uint8, int16 or int32
template <typename T>
bool write_wavfile(const char *fname, double *rawData, const WaveFileHeader& header, int numSamples){
	
	std::ofstream f(fname, std::ios::binary);
	
	// Write the file headers. RIFF-chunk:
	f << header.m_ChunkID;
	little_endian_io::write_word(f, header.m_ChunkSize);
	f << header.m_Format;
	// fmt-chunk:
	f << header.m_SubChunk1ID;
	little_endian_io::write_word(f, header.m_SubChunk1Size);
	little_endian_io::write_word(f, header.m_AudioFormat);
	little_endian_io::write_word(f, header.m_NumChannels);
	little_endian_io::write_word(f, header.m_SampleRate);
	little_endian_io::write_word(f, header.m_ByteRate);
	little_endian_io::write_word(f, header.m_BlockAlign);
	little_endian_io::write_word(f, header.m_BitsPerSample);
	// data-chunk:
	f << header.m_SubChunk2ID;
	little_endian_io::write_word(f, header.m_SubChunk2Size);
	
	// Write audio data:
	T *audioData = new T[numSamples];
	float_to_audio<double>(rawData, audioData, numSamples);
	for(int i=0; i<numSamples; i++){
		little_endian_io::write_word(f, audioData[i]);
	}
	
	f.close();
	delete[] audioData;
	
	return true;
}

int main(){
	
	int sampleRate = 44100;
	int numChannels = 2;
	float numSeconds = 2.5;
	int numSamples = sampleRate * numSeconds;
	WaveFileHeader header(sampleRate, numChannels, numSamples);
	
	// Write the audio samples
	// (We'll generate a single C4 note with a sine wave, fading from left to right)
	constexpr double two_pi = 6.283185307179586476925286766559;
	constexpr double max_amplitude = 32760;  // "volume"
	
	double frequency = 261.626;  // middle C
	
	int N = sampleRate * numSeconds;  // total number of samples
	double *rawData = new double[numSamples];
	for (int n = 0; n < numSamples; n+=2){
		double amplitude = (double)n / N * max_amplitude;
		double value     = sin( (two_pi * n * frequency) / (double)sampleRate);
		
		rawData[n] = amplitude * value;
		rawData[n+1] = (max_amplitude - amplitude) * value;
	}
	
	write_wavfile<int16_t>("sine_debug.wav", rawData, header, numSamples);
	std::cout << "Done!\n" << std::endl;
	
	delete[] rawData;
    return 0;
}
