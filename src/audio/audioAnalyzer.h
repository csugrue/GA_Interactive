#pragma once

#include "ofMain.h"

#define NUM_BANDS 256


class AudioAnalyzer{

	public:

		AudioAnalyzer();
		~AudioAnalyzer();
		
		void setup();
		void update();
		void draw();
	
		void audioReceived(ofAudioEventArgs& event); 

		// input audio data
		float * left;
		float * right;
		int 	bufferCounter;
		int 	drawCounter;
	
		// fft
		float 				* fftSmoothed;
		float normalizedValues[NUM_BANDS];
		float averageVal;
		float maxVal;
	
		// audio from music file
		ofSoundPlayer music;

};

