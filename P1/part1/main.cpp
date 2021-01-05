#include <cstring>
#include <fstream>
#include <iostream>


namespace little_endian_io{
  template <typename Word>
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof(Word)){
	for (;size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF));
    return outs;
  }
}

struct WaveFileHeader{
	
	// The RIFF-chunk
	const char *m_ChunkID = "RIFF";
	uint32_t m_ChunkSize;
	const char *m_Format = "WAVE";
	//const char *m_riffChunk = "RIFF----WAVEfmt ";
	
	// The fmt-chunk
	const char *m_SubChunk1ID = "fmt ";
	const uint32_t m_SubChunk1Size = 16;
	const uint16_t m_AudioFormat = 1;
	uint16_t m_NumChannels = 1;
	uint32_t m_SampleRate = 44100;
	uint16_t m_BitsPerSample = 8;
	const uint32_t m_ByteRate = m_SampleRate * m_NumChannels * m_BitsPerSample / 8;
	const uint16_t m_BlockAlign = m_NumChannels * m_BitsPerSample / 8;
	
	// The data-chunk
	const char *m_SubChunk2ID = "data";
	uint32_t m_SubChunk2Size;
	
	// Data...

};

bool write_wavfile(const char *fname, int32_t *pData, const WaveFileHeader& header, int numSamples){
	
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
	for(int i=0; i<numSamples; i++){
		little_endian_io::write_word(f, int(pData[i]));
	}
	
	f.close();
	return true;
}
int main(){
	
	WaveFileHeader header;
	int NumSeconds = 4;
	int NumSamples = header.m_SampleRate * header.m_NumChannels * NumSeconds;
	header.m_SubChunk2Size = NumSamples * header.m_NumChannels * header.m_BitsPerSample / 8;
	header.m_ChunkSize = 36 + header.m_SubChunk2Size;
	
	int32_t *pData = new int32_t[NumSamples];
	int32_t nValue = 0;
	for(int i = 0; i < NumSamples; i++){
		nValue += 8000000;
		pData[i] = nValue;
	}
	
	write_wavfile("part11.wav", pData, header, NumSamples);		  
	std::cout << "Done!\n" << std::endl;
	
	delete[] pData;
    return 0;
}












