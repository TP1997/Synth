#include <algorithm>
#include <iostream>
#include <math.h>

using namespace std;
int8_t float_to_int8(float s){
	float ss = clamp(s, float(0), float(INT8_MAX));
	return int8_t(ss);
}

void float_to_audio(float *rawData, int32_t *audioData, int numSamples){
	
	for(int i=0; i<numSamples; i++){
		float s = rawData[i]*INT32_MAX;
		s = clamp(s, float(INT32_MIN), float(INT32_MAX));
		audioData[i] = int32_t(s);
	}
}

int32_t float_to_int32(float rawData){
	
	double s = double(rawData*2147483647.0);
	s = clamp(s, -2147483647.0, 2147483647.0);
	
	return s;//float(s);
}

int16_t float_to_int16(float rawData){
	
	float s = rawData*INT16_MAX;
	s = std::clamp(s, float(INT32_MIN), float(INT16_MAX));
	
	return s;
}

int main(){
	//int8_t ss = float_to_int8(0.25);
	
	cout << FLOAT_MAX << endl;
}
