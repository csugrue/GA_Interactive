#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	
	ofSoundStreamSetup(0,2,this, 44100, NUM_BANDS, 4);	
	
	//setupControls();
		
	ofBackground(0,0,0);
	
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

	

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

