#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofxSTLModel.h"
#include "ofxVectorMath.h"
#include "ofxControlPanel.h"
#include "grafTagMulti.h"
#include "grafIO.h"
#include "gmlModel.h"
#include "grafCurveSmoother.h"

/*

- loads in  gml file, and draws as skeleteon
- add control panel for choosing file, loading gml, saving out model or loading in model
- edit options for points, so move through each point and adjust "twisting" or remove points that are toow close
- adjust scaling in z-depth

classes:
: gmlModelSaver
*/

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
		
		void loadTag(string name);
		
		// model
		void setModel();
		
		// controls
		void setupControls();
		void updateControls();
		
		// control panel
		ofxControlPanel	panel;
		guiTypeFileLister * gmlLister;
		guiTypeFileLister * stlLister;
		simpleFileLister lister_gml, lister_stl;
		
		
		// graf
		GmlModel		tag;
		grafIO			grafIo;
		ofxSTLModel		model;
		string currentTagName;
		grafCurveSmoother	gSmoother;
		bool bUseSmoothing;
		


};

#endif
