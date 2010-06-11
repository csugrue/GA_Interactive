#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	
	ofBackground(0,0,0);
	
	ofSoundStreamSetup(0,1,this, 44100, NUM_BANDS, 4);	

	grafAudioPlayerApp.setup();
	
	
	// for video recording
	movieSaver.setup(640,480, "GA_Audio.mov" );
	movieSaver.setCodecType( OF_QT_SAVER_CODEC_QUALITY_NORMAL );
	
	bRecordingMovie	= false;
	bUseRecorder	= false;
}



//--------------------------------------------------------------
void testApp::update(){

	ofSoundUpdate();	

	grafAudioPlayerApp.update();	
}

//--------------------------------------------------------------
void testApp::draw(){

	grafAudioPlayerApp.draw();
	
	if(bRecordingMovie)
	{
		//cout << "rec" << endl;
		int x = ofGetWidth()/2 - 320;
		int y = ofGetHeight()/2 - 240;
		ofImage img;
		img.grabScreen(x,y,640,480);
		movieSaver.addFrame(img.getPixels());
		
		ofFill();
		ofSetColor(255, 0, 0);
		ofRect(641, 481, 4, 4);
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
		if(bUseRecorder)
		{
			ofSetWindowShape(645, 485);
		}else{
			ofSetWindowShape(1024, 768);
		}
		
		
	}else if( key == 'f' || key == 'F' )
		ofToggleFullscreen();
		
	
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
