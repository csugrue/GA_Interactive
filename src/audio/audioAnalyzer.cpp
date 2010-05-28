#include "audioAnalyzer.h"

//--------------------------------------------------------------
AudioAnalyzer::AudioAnalyzer(){


}

AudioAnalyzer::~AudioAnalyzer(){

	ofRemoveListener(ofEvents.audioReceived, this, &AudioAnalyzer::audioReceived);

}

//--------------------------------------------------------------
void AudioAnalyzer::setup(){

	music.loadSound("music/17_Mirage_Quest_Brooklyn_Anthem.mp3");
	music.play();
	
	left	= new float[NUM_BANDS];
	right	= new float[NUM_BANDS];
	bufferCounter = 0;
	
	memset(normalizedValues,0.f,NUM_BANDS*sizeof(float) );
	
	ofAddListener(ofEvents.audioReceived, this, &AudioAnalyzer::audioReceived);
}


//--------------------------------------------------------------
void AudioAnalyzer::update(){

	/*if(prevAudioInput != panel.getValueI("audio_input") )
	{
		prevAudioInput = panel.getValueI("audio_input");
		
		if(prevAudioInput==1)
		{
			music.play();
		}else{
			music.stop();
		}
		
	}*/
	
	
	//ofSoundUpdate();	
	
	
	//--- fft data
	float * fftValues = ofSoundGetSpectrum(NUM_BANDS);
	
	averageVal = 0;
	maxVal	 = 0;
	
	// calculate the maxValue
	for(int i=0; i<NUM_BANDS; i++){
		if(fftValues[i] > maxVal) maxVal = fftValues[i];
	}
	
	// normalize values using the max
	for(int i=0; i<NUM_BANDS; i++){
		normalizedValues[i] = fftValues[i] / maxVal;
		averageVal += normalizedValues[i];
	}
	
	// calculate the average value
	averageVal /= (float)NUM_BANDS;
}

//--------------------------------------------------------------
void AudioAnalyzer::draw(){

	ofEnableAlphaBlending();
	
	//---- draw audio analysis
	ofFill();
	
	// draw the left input:
	ofSetColor(0x333333);
	glPushMatrix();
	glTranslatef(10,10,0);
	ofRect(0,0,256,200);
	ofSetColor(0xFFFFFF);
	for (int i = 0; i < 256; i++)
		ofLine(i,100,i,100+left[i]*100.0f);
	glPopMatrix();
	
	
	// draw the right input:
	ofSetColor(0x333333);
	glPushMatrix();
		glTranslatef(276,10,0);
		ofRect(0,0,256,200);
		ofSetColor(0xFFFFFF);
		for (int i = 0; i < 256; i++)
			ofLine(i,100,i,100+right[i]*100.0f);
	glPopMatrix();
	
	
	// fft analysis
	glPushMatrix();
		glTranslatef(10,230,0);
	
		ofSetColor(0x333333);
		ofRect(-1,-1,258,102);
	
		ofSetColor(255,0,0, 100);
		ofLine(0,100-(100*averageVal),NUM_BANDS,100-(100*averageVal));
	
		ofSetColor(0xFFFFFF);
		for(int i=0; i<NUM_BANDS; i++)
			ofLine(i,100,i,100-normalizedValues[i]*100.0f);
		
	glPopMatrix();
}

void AudioAnalyzer::audioReceived(ofAudioEventArgs& event)
{
	// samples are "interleaved"
	for (int i = 0; i < event.bufferSize; i++){
		left[i] = event.buffer[i*2];
		right[i] = event.buffer[i*2+1];
	}
	bufferCounter++;
	
}

