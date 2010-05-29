#include "audioAnalyzer.h"

//--------------------------------------------------------------
AudioAnalyzer::AudioAnalyzer(){
	
	bSetup = false;

}

AudioAnalyzer::~AudioAnalyzer(){

	/*delete [] eqFunction;
	delete [] eqOutput;
	delete [] ifftOutput;
	delete [] fftOutput;
	delete [] audioInput;*/
	//delete fft;
	
}



//--------------------------------------------------------------
void AudioAnalyzer::setup(){


	bufferSize = NUM_BANDS;
	
	fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_BARTLETT);
	
	audioInput	= new float[bufferSize];
	fftOutput	= new float[fft->getBinSize()];
	eqFunction	= new float[fft->getBinSize()];
	eqOutput	= new float[fft->getBinSize()];
	ifftOutput	= new float[bufferSize];
	
	bufferCounter = 0;
	
	// this describes a linear low pass filter
	for(int i = 0; i < fft->getBinSize(); i++)
		eqFunction[i] = .1*powf((float)i,1.1);//(float) (fft->getBinSize() - i) / (float) fft->getBinSize();


	//memset(normalizedValues,0.f,NUM_BANDS*sizeof(float) );
	//memset(normInputValues,0.f,NUM_BANDS*sizeof(float) );
	//memset(audioInput,0.f,NUM_BANDS*sizeof(float));
	
	aubio.setup();

	bSetup = true;

}


//--------------------------------------------------------------
void AudioAnalyzer::update(){

	fft->setSignal( audioInput );
	
	memcpy(fftOutput, fft->getAmplitude(), sizeof(float) * fft->getBinSize());
	
	for(int i = 0; i < fft->getBinSize(); i++)
		eqOutput[i] = fftOutput[i] * eqFunction[i];
	
	fft->setPolar(eqOutput, fft->getPhase());
	
	fft->clampSignal();
	memcpy(ifftOutput, fft->getSignal(), sizeof(float) * fft->getSignalSize());
	
	
	averageVal = 0;
	maxVal	 = 0;
		
	/*
	// normalize values using the max
	int binSize = fft->getBinSize();
	float * amps = fft->getAmplitude();
	
	for(int i=0; i<binSize; i++){
		if( amps[i] > maxVal ) maxVal = amps[i];
	}
	
	for(int i=0; i<binSize; i++){
		normalizedValues[i] = amps[i];// / maxVal;
		averageVal += fabs(audioInput[i]);//normalizedValues[i];		
	}
	
	// calculate the average value
	averageVal /= (float)binSize;*/
	
	aubio.processAudio(audioInput, NUM_BANDS);
}

//--------------------------------------------------------------
void AudioAnalyzer::draw(){

	int plotHeight = 128;
	
	ofEnableAlphaBlending();
	
	ofSetColor(0xffffff);
	ofPushMatrix();
	
	glTranslatef(16, 16, 0);
	ofDrawBitmapString("Audio Input", 0, 0);
	plot(audioInput, bufferSize, plotHeight / 2, 0);
	
	glTranslatef(0, plotHeight + 16, 0);
	ofDrawBitmapString("FFT Output", 0, 0);
	plot(fftOutput, fft->getBinSize(), -plotHeight, plotHeight / 2);
	
	ofPushMatrix();
	glTranslatef(fft->getBinSize(), 0, 0);
	ofDrawBitmapString("EQd FFT Output", 0, 0);
	plot(eqOutput, fft->getBinSize(), -plotHeight, plotHeight / 2);
	ofPopMatrix();
	
	glTranslatef(0, plotHeight + 16, 0);
	ofDrawBitmapString("IFFT Output", 0, 0);
	plot(ifftOutput, fft->getSignalSize(), plotHeight / 2, 0);
	
	ofPopMatrix();
	
	//---- draw audio analysis
	/*ofFill();
	
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
	ofSetColor(255,255,255,255);
	fft->draw(10, 230, fft->getSignalSize(), 200);
	
	ofSetColor(255,0,0, 100);
	ofLine(10,430-(averageVal*400),NUM_BANDS+10,430-(averageVal*400));
	
	ofSetColor(255,255,0, 100);
	ofLine(10,430-(aubio.amplitude*400),NUM_BANDS+10,430-(aubio.amplitude*400));
	
	//ofDrawBitmapString( "pitch is : " + ofToString((int)aubio.pitch) + "\namplitude is : " + ofToString(aubio.amplitude,3), 10,500);

	return;
	
	glPushMatrix();
		glTranslatef(10,230,0);
	
		ofSetColor(0x333333);
		ofRect(-1,-1,258,102);
	
		ofSetColor(255,0,0, 100);
		ofLine(0,100-(100*averageVal),NUM_BANDS,100-(100*averageVal));
	
		ofSetColor(0xFFFFFF);
		for(int i=0; i<NUM_BANDS; i++)
			ofLine(i,100,i,100-normalizedValues[i]*100.0f);
		
	glPopMatrix();*/
}

void AudioAnalyzer::plot(float* array, int length, float scale, float offset)
{
	float plotHeight = 128;
	
	ofFill();
	ofSetColor(0,0,0,200);
	ofRect(0, 0, length, plotHeight);
	
	ofSetColor(255,255,255,255);

	ofNoFill();
	ofRect(0, 0, length, plotHeight);
	
	glPushMatrix();
		glTranslatef(0, plotHeight / 2 + offset, 0);
		ofBeginShape();
			for (int i = 0; i < length; i++)
				ofVertex(i, array[i] * scale);
		ofEndShape();
	glPopMatrix();
}
