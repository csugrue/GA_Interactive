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
#include "ofxFBOTexture.h"
#include "grafArchitecture.h"

#define TAG_DIRECTORY	"tags/"
#define FBO_W	1280
#define FBO_H	800

enum{ PLAY_MODE_LOAD, PLAY_MODE_PLAY };

class GrafPlayerApp{

	public:

		GrafPlayerApp();
		~GrafPlayerApp();
		
		void setup();
		
		void update();
		void updateParticles();
		void updateTransition( int type);
		void updateAudio();
		void updateArchitecture();
		
		void draw();

		void keyPressed(ofKeyEventArgs & event);
		void keyReleased(ofKeyEventArgs & event);
		void mouseMoved(ofMouseEventArgs & event );
		void mouseDragged(ofMouseEventArgs & event );
		void mousePressed(ofMouseEventArgs & event );
		void mouseReleased(ofMouseEventArgs & event );
	
		void preLoadTags();
		void loadTags();
	
		void resetPlayer(int next);
		void nextTag(int dir=1);
		void clearAll();
	
		void setupControlPanel();
		void updateControlPanel();
		
		string getCurrentTagName();
	
	
		void drawAudioLine();
		
		//---------- FBO
		ofxFBOTexture	fbo;
		
		//---------- interactive architecture
		GrafArchitecture		archPhysics;		// physics / architecture effects
		
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
		int currentTagID;							// id of the current tag being played 

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
		ofTrueTypeFont		fontS;					// fonts for drawing info to screen
		ofTrueTypeFont		fontL;
		ofTrueTypeFont		fontSS;
		int					prevStroke;				// for multi stroke tags, to help particle animation
		float				fogStart,fogEnd;		// fog settings
		float				rotFixTime;				// timer for making rotation move to particular position in a fixed time frame
		
			
		//---------- application settings
        bool            bShiftOn;
		int             screenW,screenH;
		float			lastX,lastY;
		ofxThreadedImageSaver imgsaver;
		ofImage				imageMask;
		float					dt;
		float					waitTimer;			// counter to pause after animation finished
		float					waitTime;			// time to pause
	

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
		bool				bUseAudio;
		bool				bUseArchitecture;
		
		int					mode;				// current play mode PLAY_MODE_LOAD for loading, PLAY_MODE_PLAY for normal playback
	
		// files opening
		ofxFileDialog dialog;
		vector <string> resultString;
	
	
	
};

#endif
