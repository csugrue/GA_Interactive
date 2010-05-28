#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "grafPlayerApp.h"

/*
Graffiti Analysis 2.0: Audio Interactive
2010

Analyzes audio input and plays <gml> tags that respond to fft data.

*/

#define NUM_BANDS 256

class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);
		
		
		GrafPlayerApp	grafAudioPlayerApp;
		
		// controls
		//void setupControls();
		//void updateControls();
		
		// control panel
		/*ofxControlPanel	panel;
		guiTypeFileLister * gmlLister;
		simpleFileLister lister_gml;
		int prevAudioInput;*/

};

#endif
