#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	
	ofBackground(0,0,0);
	
	ofSoundStreamSetup(0,1,this, 44100, NUM_BANDS, 4);	

	grafInteractiveApp.setup();
	
	
	// for video recording
	movieSaver.setup(640,480, "output/video/GA.mov" );
	movieSaver.setCodecType( OF_QT_SAVER_CODEC_QUALITY_NORMAL );
	
	bRecordingMovie	= false;
	bUseRecorder	= false;
	
	bShowMouse = true;

}



//--------------------------------------------------------------
void testApp::update(){

	ofSoundUpdate();	

	grafInteractiveApp.update();	
}

//--------------------------------------------------------------
void testApp::draw(){

	grafInteractiveApp.draw();
	
	if(bUseRecorder)
	{
		int x = ofGetWidth()/2 - 320;
		int y = ofGetHeight()/2 - 240;
		ofNoFill();
		ofRect(x-1,y-1,642, 482);
	}
	
	if(bRecordingMovie)
	{
		int x = ofGetWidth()/2 - 320;
		int y = ofGetHeight()/2 - 240;
		ofImage img;
		img.grabScreen(x,y,640,480);
		movieSaver.addFrame(img.getPixels());
		
		ofFill();
		ofSetColor(255, 0, 0);
		ofRect(x+641, y+481, 4, 4);
		ofSetColor(255, 255, 255);		
	}
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	if( key == OF_KEY_F4 )
	{
		if(bUseRecorder)
		{
			if(bRecordingMovie) movieSaver.finishMovie();
			bRecordingMovie = !bRecordingMovie;
		}
	}
	
	else if( key == OF_KEY_F3 )
	{
		bUseRecorder = !bUseRecorder;
		
		
	}else if( key == 'f' || key == 'F' )
		ofToggleFullscreen();
	else if( key == 'm')
	{
		bShowMouse = !bShowMouse;
		if(!bShowMouse) ofHideCursor();
		else ofShowCursor();
		
	}	
	
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
	if( modifier == GLUT_ACTIVE_SHIFT) grafInteractiveApp.bShiftOn = true;
	else grafInteractiveApp.bShiftOn = false;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels){	
	// samples are "interleaved"
	if(grafInteractiveApp.audio.bSetup && grafInteractiveApp.bUseAudio)
	{
	memcpy(grafInteractiveApp.audio.audioInput, input, sizeof(float) * (NUM_BANDS));
		grafInteractiveApp.audio.bufferCounter++;
	}
}
