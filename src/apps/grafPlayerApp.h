#ifndef _GRAF_PLAYER_APP
#define _GRAF_PLAYER_APP


#include "ofMain.h"
#include "grafTagMulti.h"
#include "grafIO.h"
#include "grafPlayer.h"
#include "grafCurveSmoother.h"
#include "grafDrawer.h"
#include "pocoDirectoryLister.h"
#include "grafVParticleField.h"
#include "ofxControlPanel.h"
#include "ofxXmlSettings.h"
#include "ofThreadedImage.h"
#include "audioAnalyzer.h"
#include "ofxFileDialog.h"
#include "dropParticles.h"

#define TAG_DIRECTORY	"tags/"

enum{ PLAY_MODE_LOAD, PLAY_MODE_PLAY };

class GrafPlayerApp{

	public:

		GrafPlayerApp();
		~GrafPlayerApp();
		void setup();
		void update();
		void draw();

		void keyPressed(ofKeyEventArgs & event);
		void keyReleased(ofKeyEventArgs & event);
		void mouseMoved(ofMouseEventArgs & event );
		void mouseDragged(ofMouseEventArgs & event );
		void mousePressed(ofMouseEventArgs & event );
		void mouseReleased(ofMouseEventArgs & event );
	
		/*void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);*/
		
		void preLoadTags();
		void loadTags();
	
		void resetPlayer(int next);
		void nextTag(int dir=1);
		void clearAll();
	
		void setupContolPanel();
		void updateConrolPanel();
		
		string getCurrentTagName();
	
	
		void drawAudioLine();
		
		
		//---------- audio analysis
		AudioAnalyzer	audio;
		
		//---------- effects
		DropParticles	drops;
		
		
		//---------- loading data
		grafIO					gIO;					// gml loader/saver
		ofxPocoDirectoryLister	dirLister;				// searches directory for gml files
		string					myTagDirectory;			// directory to load from (changeable in data/settings/directorySettings.xml
		int						totalToLoad;			// totalFiles found to load
		vector<string>			filesToLoad;			// list of all file paths
		vector<string>			filenames;				// list of corresponding file names (sans .gml)
	
	
	
		//---------- gml tags
		vector<grafTagMulti> tags;						
	
		//---------- player to animate the tag 
		grafPlayer	myTagPlayer;					
		
		//---------- smooths curve for nice drawing
		grafCurveSmoother smoother;
	
		//---------- drawing
		grafDrawer			drawer;					// draws thick time stroked line
		grafVParticleField	particleDrawer;			// draws and animates particles
		float				rotationY;				// y rotation for current tag
		ofPoint				tagPosVel;				// vel for moving tag around on screen
		float				tagMoveForce;			// force for moving tag around
		//ofTrueTypeFont		fontS;					// fonts for drawing info to screen
		//ofTrueTypeFont		fontL;
		//ofTrueTypeFont		fontSS;
		int					prevStroke;				// for multi stroke tags, to help particle animation
		float				fogStart,fogEnd;		// fog settings
		float				dt;
		ofImage				imageMask;
	
	
		//---------- id of the current tag being played
		int currentTagID;
	
		//---------- 
		float					waitTimer;			// counter to pause after animation finished
		float					waitTime;			// time to pause
		
		//---------- application settings
        bool            bShiftOn;
		int             screenW,screenH;
		float			lastX,lastY;
		ofxThreadedImageSaver imgsaver;
	

		//---------- controls
		ofxControlPanel		panel;
		bool				bShowPanel;			// toggle panel on/off
		bool				bRotating;			// toggle auto rotation on/off
		bool				bShowName;			// toggle filename displa
		bool				bShowTime;			// toggle time display
		bool				bUseFog;			// toggle fog
		bool				bUseMask;			// toggle edge masking
		bool				bTakeScreenShot;	
		bool				bUseGravity;
	
		int					mode;				// current play mode PLAY_MODE_LOAD for loading, PLAY_MODE_PLAY for normal playback
	
		// files opening
		ofxFileDialog dialog;
		vector <string> resultString;
	
	
	
};

#endif
