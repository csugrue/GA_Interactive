#pragma once

#include "ofMain.h"
#include "ofxFft.h"
#include "aubioAnalyzer.h"

#define NUM_BANDS 512


class AudioAnalyzer{

	public:

		AudioAnalyzer();
		~AudioAnalyzer();
		
		void setup();
		void update();
		void draw();
	
		void plot(float* array, int length, float scale, float offset);
		
		int 	bufferCounter;
		int 	drawCounter;
	
		// fft
		//float normalizedValues[NUM_BANDS];
		//float normInputValues[NUM_BANDS];
		float averageVal;
		float maxVal;
		
		ofxFft	* fft;
		float	* eqFunction;
		float	* eqOutput;
		float	* ifftOutput;
		float	* fftOutput;
		float	* audioInput;
		
		int bufferSize;
		
		bool bSetup;
		
		// aubio stuff
		aubioAnalyzer	aubio;

};

