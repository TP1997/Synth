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


//specify sample bit count as the template parameter
//can be uint8, int16 or int32
//template <typename T>
void write_wavfile(const char *fname, const int *samples1, const int *samples2, const int N){
	std::ofstream f(fname, std::ios::binary);
	
	// Write the file headers
	f << "RIFF----WAVEfmt ";     // ChunkSize, (chunk size to be filled in later)
	write_word( f,     16, 4 );  // Subchunk1Size, no extension data 
	write_word( f,      1, 2 );  // AudioFormat, PCM - integer samples
	write_word( f,      2, 2 );  // NumChannels, two channels (stereo file) 2
	write_word( f,  44100, 4 );  // SampleRate, samples per second (Hz)
	write_word( f, 176400, 4 );  // ByteRate, (Sample Rate * BitsPerSample * Channels) / 8
	write_word( f,      4, 2 );  // BlockAlign, data block size (size of two integer samples, one for each channel, in bytes)4
	write_word( f,     16, 2 );  // BitsPerSample, number of bits per sample (use a multiple of 8)16
	
	// Write the data chunk header
	size_t data_chunk_pos = f.tellp();
	f << "data----";  // Subchunk2Size, (chunk size to be filled in later)
	
	// Fill the data
	for(int n=0; n<N; n++){
		write_word( f, samples1[n], 2 );
		write_word( f, samples2[n], 2 );
	}
	// (We'll need the final file size to fix the chunk sizes above)
	size_t file_length = f.tellp();
	
	// Fix the data chunk header to contain the data size
	f.seekp( data_chunk_pos + 4 );
	write_word( f, file_length - data_chunk_pos + 8 );
	
	// Fix the file header to contain the proper RIFF chunk size, which is (file size - 8) bytes
	f.seekp( 0 + 4 );
	write_word( f, file_length - 8, 4 );
	
}

void write_wavfile2(const char *fname, const int16_t *samples1, const int16_t *samples2, const int N){
	
	const char* ChunkID 		= "RIFF";
	int32_t ChunkSize; //!
	const char* Format 			= "WAVE";
	
	const char* Subchunk1ID 	= "fmt ";
	const int32_t Subchunk1Size = 16;
	const int16_t AudioFormat	= 1;
	int16_t NumChannels 		= 2;
	int32_t SampleRate 		  	= 44100; 
	int32_t ByteRate;
	int16_t BlockAlign;
	int16_t BitsPerSample		= sizeof(int16_t) * 8;
	ByteRate = SampleRate * NumChannels * BitsPerSample / 8;
	BlockAlign = NumChannels * BitsPerSample / 8;
	
	const char* Subchunk2ID 	= "data";
	int32_t Subchunk2Size = N * NumChannels * BitsPerSample / 8;
	ChunkSize = 36 + Subchunk2Size;
	
	std::ofstream f(fname, std::ios::binary);
	
	// Write the file headers
	f << ChunkID;
	write_word(f, ChunkSize);
	f << Format << Subchunk1ID;

	write_word( f, Subchunk1Size);  // Subchunk1Size, no extension data 
	write_word( f, AudioFormat);  // AudioFormat, PCM - integer samples
	write_word( f, NumChannels);  // NumChannels, two channels (stereo file) 2
	write_word( f, SampleRate);  // SampleRate, samples per second (Hz)
	write_word( f, ByteRate);  // ByteRate, (Sample Rate * BitsPerSample * Channels) / 8
	write_word( f, BlockAlign);  // BlockAlign, data block size (size of two integer samples, one for each channel, in bytes)4
	write_word( f, BitsPerSample);  // BitsPerSample, number of bits per sample (use a multiple of 8)16
	
	// Write the data chunk header
	f << Subchunk2ID;
	write_word(f, Subchunk2Size);
	
	// Fill the data
	for(int n=0; n<N; n++){
		write_word(f, samples1[n]);
		write_word(f, samples2[n]);
	}
	
	f.close();
}

//###############################################3

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

template<typename T>
void write_wavfile3(const char *fname, const T *samples1, const T *samples2, const int N, const WaveFileHeader& wav_header){
	
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
		write_word( f, samples1[n], 2 );
		write_word( f, samples2[n], 2 );
	}
	
	f.close();
}

int main(){
		
	// Write the audio samples
	// (We'll generate a single C4 note with a sine wave, fading from left to right)
	constexpr double two_pi = 6.283185307179586476925286766559;
	constexpr double max_amplitude = 32760;  // "volume"
	
	double hz        = 44100;    // samples per second
	double frequency = 261.626;  // middle C
	double seconds   = 2.5;      // time
	int channels = 2;
	
	int N = hz * seconds;  // total number of samples
	int16_t *samples1 = new int16_t[N]; 
	int16_t *samples2 = new int16_t[N];
	for (int n = 0; n < N; n+=1){
		double amplitude = (double)n / N * max_amplitude;
		double value     = sin( (two_pi * n * frequency) / hz );
		
		samples1[n] = (int16_t)(amplitude  * value);
		samples2[n] = (int16_t)((max_amplitude - amplitude) * value);
	}
	
	// WaveFileHeader header(N, channels, hz);
	//write_wavfile2("test3.wav", samples1, samples2, N);
	WaveFileHeader header(N, channels, hz, sizeof(int16_t));
	write_wavfile3<int16_t>("test4.wav", samples1, samples2, N, header);
	
	cout << "Done!" << endl;
	return 0;
}
