#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	
	ofBackground(0,0,0);
	
	ofSoundStreamSetup(0,1,this, 44100, NUM_BANDS, 4);	

	grafAudioPlayerApp.setup();
	
}



//--------------------------------------------------------------
void testApp::update(){

	ofSoundUpdate();	

	grafAudioPlayerApp.update();	
}

//--------------------------------------------------------------
void testApp::draw(){

	grafAudioPlayerApp.draw();
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	int modifier = glutGetModifiers();
	if( modifier == GLUT_ACTIVE_SHIFT) grafAudioPlayerApp.bShiftOn = true;
	else grafAudioPlayerApp.bShiftOn = false;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels){	
	// samples are "interleaved"
	if(grafAudioPlayerApp.audio.bSetup)
	{
	memcpy(grafAudioPlayerApp.audio.audioInput, input, sizeof(float) * (NUM_BANDS));
		grafAudioPlayerApp.audio.bufferCounter++;
	}
}
