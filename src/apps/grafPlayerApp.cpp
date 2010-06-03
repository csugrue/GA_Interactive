#include "grafPlayerApp.h"

//--------------------------------------------------------------
GrafPlayerApp::GrafPlayerApp(){


}

GrafPlayerApp::~GrafPlayerApp(){

	ofRemoveListener(ofEvents.mouseMoved, this, &GrafPlayerApp::mouseMoved);
	ofRemoveListener(ofEvents.mousePressed, this, &GrafPlayerApp::mousePressed);
	ofRemoveListener(ofEvents.mouseReleased, this, &GrafPlayerApp::mouseReleased);
	ofRemoveListener(ofEvents.mouseDragged, this, &GrafPlayerApp::mouseDragged);
	ofRemoveListener(ofEvents.keyPressed, this, &GrafPlayerApp::keyPressed);
	ofRemoveListener(ofEvents.keyReleased, this, &GrafPlayerApp::keyReleased);

}
//--------------------------------------------------------------
void GrafPlayerApp::setup(){

	ofAddListener(ofEvents.mouseMoved, this, &GrafPlayerApp::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &GrafPlayerApp::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &GrafPlayerApp::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &GrafPlayerApp::mouseDragged);
	ofAddListener(ofEvents.keyPressed, this, &GrafPlayerApp::keyPressed);
	ofAddListener(ofEvents.keyReleased, this, &GrafPlayerApp::keyReleased);

	mode			= PLAY_MODE_LOAD;
	screenW			= 1024;
	screenH			= 768;
	lastX			= 0;
	lastY			= 0;
	bShowPanel		= true;
	bRotating		= false;
	bShowName		= false;
	bShowTime		= false;
	bUseFog			= true;
	bUseMask		= true;
	bTakeScreenShot = false;
	bUseGravity		= true;
	prevStroke		= 0;
	
	currentTagID	= 0;
	waitTime		= 2.f;
	waitTimer		= waitTime;
	rotationY		= -45;
	tagMoveForce	= .1;
	tagPosVel.set(0,0,0);
	myTagDirectory = TAG_DIRECTORY;
	
	//fontSS.loadFont("fonts/frabk.ttf",9);
	//fontS.loadFont("fonts/frabk.ttf",14);
	//fontL.loadFont("fonts/frabk.ttf",22);
	imageMask.loadImage("images/mask.jpg");
	
	float FogCol[3]={0,0,0};
    glFogfv(GL_FOG_COLOR,FogCol);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_DENSITY, 0.05f);
	fogStart = 370;
    fogEnd   = 970;
	
	ofxXmlSettings xml;
	xml.loadFile("data/settings/directorySettings");
	myTagDirectory = xml.getValue("directory", TAG_DIRECTORY);
	
	setupContolPanel();
	updateConrolPanel();
	
	preLoadTags();
	particleDrawer.setup(screenW,screenH);
	
	
	audio.setup();
	
	//drops.setup(30);
	
	// temp
	panel.setSelectedPanel("Audio Settings");

}

static float lastTime = 0.f;

//--------------------------------------------------------------
void GrafPlayerApp::update(){

	dt  = ofGetElapsedTimef()-lastTime;
	lastTime  = ofGetElapsedTimef();
	
	
	bool bTrans = false;
	
	if( mode == PLAY_MODE_LOAD )
	{
		loadTags();
	}
	else if( mode == PLAY_MODE_PLAY && tags.size() > 0 )
	{
		
		
		//---- set drawing data for render
		if( drawer.bSetupDrawer )
			drawer.setup( &tags[currentTagID], tags[currentTagID].distMax );

		
		
		//---- update tag playing state
		if( !myTagPlayer.bDonePlaying )					
		{
			// normal play, update tag
			myTagPlayer.update(&tags[currentTagID]);
		
		}else if( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && waitTimer > 0)			   
		{
			// pause time after drawn, before fades out
			waitTimer -= dt;
		}
		else if ( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && (drawer.alpha > 0 || particleDrawer.alpha > 0))			  		
		{
			
			bTrans = true;
			
			// fade away, dissolve
			//
			
			float deform_frc = panel.getValueF("wave_deform_force");
			float line_amp_frc = panel.getValueF("line_width_force");
			
			if( panel.getValueB("use_wave_deform") ) drawer.transitionDeform( dt,deform_frc, audio.audioInput, NUM_BANDS);
			if( panel.getValueB("use_line_width") ) drawer.transitionLineWidth( dt,audio.averageVal*line_amp_frc);
			if( panel.getValueB("use_bounce") ) drawer.transitionBounce( dt,audio.averageVal*line_amp_frc);
			if( drawer.pctTransLine < .1 ) drawer.pctTransLine += .001;
			drawer.prelimTransTime += dt;
			
			if(drawer.prelimTransTime > panel.getValueI("wait_time") )
			{
				// do average transition
				drawer.transition(dt,.015);
				
				if( bUseGravity ) 
					particleDrawer.fall(dt);
			
				if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
			}
			
		}
		else if (  !myTagPlayer.bPaused && myTagPlayer.bDonePlaying )							
		{
			// setup for next tag
			resetPlayer(1);
		}
	
		
		//---- update the particle field
		int lastStroke = myTagPlayer.getCurrentStroke();
		int lastPoint  = myTagPlayer.getCurrentId();
		
		if( prevStroke != lastStroke )	myTagPlayer.bReset = true; 
		if( lastPoint <= 0 )			myTagPlayer.bReset = true;
		if( tags[currentTagID].myStrokes[ lastStroke].pts.size()-1 == lastPoint ) myTagPlayer.bReset = true;
		
		
		
		//---------- AUDIO applied
		if(panel.getValueB("use_p_size") ) 
			particleDrawer.updateParticleSizes(audio.eqOutput,audio.averageVal, NUM_BANDS,panel.getValueF("particle_size_force") );
		
		particleDrawer.setDamping( panel.getValueF("P_DAMP") );
		//if(panel.getValueB("use_p_damp") ) 
		//	particleDrawer.updateDampingFromAudio(panel.getValueF("p_audio_damp")*audio.averageVal*.1);
		
		if(!bTrans && panel.getValueB("use_p_amp") ) 
			particleDrawer.updateParticleAmpli(audio.ifftOutput,audio.averageVal, NUM_BANDS,panel.getValueF("outward_amp_force") );
		
		// update particle drops (audio stuff);
		drops.update(dt);
		
		//---------
		
		
		
		//--------- PARTICLES
		particleDrawer.update( myTagPlayer.getCurrentPoint(),myTagPlayer.getVelocityForTime(&tags[currentTagID]),  dt,  myTagPlayer.bReset);
		myTagPlayer.bReset = false; // important so no particle error on first stroke

		//--------- 
		
		
		prevStroke		= myTagPlayer.getCurrentStroke();		
		
		// update rotation
		if(bRotating) rotationY += panel.getValueF("ROT_SPEED")*dt;
		
		// update pos / vel
		tags[currentTagID].position.x += tagPosVel.x;
		tags[currentTagID].position.y += tagPosVel.y;
		
		tagPosVel.x -= .1*tagPosVel.x;
		tagPosVel.y -= .1*tagPosVel.y;
		
		cout << "rotationY " << rotationY << endl;
		
		
	}
	
	if( bShowPanel ) updateConrolPanel();
	
	// update audio
	audio.update();
}


//--------------------------------------------------------------
void GrafPlayerApp::draw(){

	ofEnableAlphaBlending();
	ofSetColor(255,255,255,255);

	if( mode == PLAY_MODE_LOAD )
	{
		;
	}
	else if( mode == PLAY_MODE_PLAY )
	{
	
		glPushMatrix();
		
		
			if( bUseFog )
			{
				glFogf(GL_FOG_START, fogStart );
				glFogf(GL_FOG_END, fogEnd );
				glEnable(GL_FOG);				
			}
		
			glTranslatef(screenW/2, screenH/2, 0);
		
			glScalef(tags[currentTagID].position.z,tags[currentTagID].position.z,tags[currentTagID].position.z);
		
			glTranslatef(tags[currentTagID].position.x,tags[currentTagID].position.y,0);
			

			
			glPushMatrix();
		
				glRotatef(tags[currentTagID].rotation.x,1,0,0);
				glRotatef(tags[currentTagID].rotation.y+rotationY,0,1,0);
				glRotatef(tags[currentTagID].rotation.z,0,0,1);
		
				glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
				glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,-tags[currentTagID].center.z);
		
				glDisable(GL_DEPTH_TEST);
				
				particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  screenW,  screenH);
				drops.draw();
				
				
				
				glEnable(GL_DEPTH_TEST);
				
				ofSetColor(255,0,0);
				glPushMatrix();
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					//drawAudioLine();
				glPopMatrix();
	
				
				glPushMatrix();
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
					
				glPopMatrix();
		
				glPushMatrix();
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					
					//drawer.drawBoundingBox( tags[currentTagID].min, tags[currentTagID].max, tags[currentTagID].center );
				glPopMatrix();
				
				
		
			glPopMatrix();
		
			
		
		glPopMatrix();
		
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
		

	
	
	}
	
	if(bUseMask)
	{
		ofEnableAlphaBlending();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		imageMask.draw(0,0,ofGetWidth(),ofGetHeight());
	}
	
	
	if( mode == PLAY_MODE_PLAY )
	{
		ofSetColor(255,255,255,255);
		//if( bShowName && tags.size() > 0 ) fontS.drawString( tags[ currentTagID ].tagname, 10,ofGetHeight()-30 );
		if( bShowTime && tags.size() > 0 )
		{
			float time = myTagPlayer.getCurrentTime();
			//float wd = fontL.stringWidth( ofToString( time,0) ) / 10.f;
			//wd = 10*(int)(wd);
			
			//fontL.drawString(ofToString(time,2), ofGetWidth()-wd-70, ofGetHeight()-30);
		}
	
	}


	ofEnableAlphaBlending();
	
	if(bShowPanel){
		panel.draw();
		//fontSS.drawString("x: toggle control panel  |  p: pause/play  |  s: screen capture  |  m: toggle mouse  |  f: toggle fullscreen  |  h: toggle home  |  arrows: next/prev  |  esc: quit", 90, ofGetHeight()-50);
		//fontSS.drawString("left mouse: alter position  |  left+shift mouse: zoom  |  right mouse: rotate y  |  right+shift mouse: rotate x", 220, ofGetHeight()-30);
		
		if( panel.getSelectedPanelName() == "Audio Settings" )
		{
			audio.draw();
		}
		
	}
	

	// screenshots
	if(bTakeScreenShot)
	{
		imgsaver.grabScreen(0,0,ofGetWidth(),ofGetHeight() );
		
		dirLister.setPath( ofToDataPath(myTagDirectory + tags[currentTagID].tagname + "/", true));
		dirLister.setExtensionToLookFor("png");
		int num = dirLister.getNumberOfFiles();
		imgsaver.saveThreaded( myTagDirectory + tags[currentTagID].tagname + "/"+tags[currentTagID].tagname+"_"+ofToString(num+1)+".png");
		
		bTakeScreenShot = false;
	}
	
}


//--------------------------------------------------------------
void GrafPlayerApp::keyPressed (ofKeyEventArgs & event){


	
    switch(event.key){

  		case 'x': bShowPanel=!bShowPanel; break;
		case 'p': 
			myTagPlayer.bPaused = !myTagPlayer.bPaused;
			bRotating = !myTagPlayer.bPaused;
			panel.setValueB("ROTATE",bRotating);
			panel.setValueB("PLAY",!myTagPlayer.bPaused);
			break;
			
			
		case OF_KEY_RIGHT:  resetPlayer(1); break;
        case OF_KEY_LEFT:   resetPlayer(-1); break;
		
		case 's': bTakeScreenShot = true; break;
			
		 
		default:
  			break;

  }

	if( event.key == '/')
	{
		int randomP = particleDrawer.PS.getIndexOfRandomAliveParticle();//ofRandom( 0, particleDrawer.PS.numParticles );
		ofPoint pPos = ofPoint(particleDrawer.PS.pos[randomP][0],particleDrawer.PS.pos[randomP][1],particleDrawer.PS.pos[randomP][2]);
		ofPoint pVel = ofPoint(particleDrawer.PS.vel[randomP][0],particleDrawer.PS.vel[randomP][1],particleDrawer.PS.vel[randomP][2]);
		drops.createRandomDrop( pPos, pVel, particleDrawer.PS.sizes[randomP] );
		
	}
	

}

//--------------------------------------------------------------
void GrafPlayerApp::keyReleased(ofKeyEventArgs & event){

}

//--------------------------------------------------------------
void GrafPlayerApp::mouseMoved(ofMouseEventArgs & event ){

	lastX   = event.x;
	lastY   = event.y;
}

//--------------------------------------------------------------
void GrafPlayerApp::mouseDragged(ofMouseEventArgs & event ){

	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseDragged(event.x,event.y,event.button);
	}
	else{
		if( event.button == 0 )
		{
			if(!bShiftOn)
			{
				
				tagPosVel.x +=  tagMoveForce * (event.x-lastX);
				tagPosVel.y +=  tagMoveForce * (event.y-lastY);
			}else if(tags.size() > 0){
				tags[currentTagID].position.z += .01 * (event.y-lastY);
				tags[currentTagID].position.z = MAX(tags[currentTagID].position.z,.01);
			}
			
		
		}else{
			if( tags.size() > 0 && !bShiftOn) tags[currentTagID].rotation.y += (event.x-lastX);
			else if( tags.size() > 0 && bShiftOn) tags[currentTagID].rotation.x += (event.y-lastY);
		}
	}	
	
	
	lastX   = event.x;
	lastY   = event.y;

}

//--------------------------------------------------------------
void GrafPlayerApp::mousePressed(ofMouseEventArgs & event ){

    if( bShowPanel ) panel.mousePressed(event.x,event.y,event.button);

}

//--------------------------------------------------------------
void GrafPlayerApp::mouseReleased(ofMouseEventArgs & event ){

	if( bShowPanel && !panel.minimize) 
	{
		panel.mouseReleased();
	}
	 


}

//--------------------------------------------------------------
void GrafPlayerApp::resetPlayer( int next)
{
	if(tags.size() <= 0 ) return;
	
	myTagPlayer.reset();
	
	nextTag(next);
	
	drawer.setup( &tags[currentTagID], tags[currentTagID].distMax );
	
	particleDrawer.reset();
	
	waitTimer = waitTime;
	
	rotationY = 0;
	
	tagPosVel.set(0,0,0);
	
	prevStroke = 0;

	
}
//--------------------------------------------------------------
void GrafPlayerApp::nextTag(int dir)
{
	
	if(dir==1)
	{
		currentTagID++;
		currentTagID %= tags.size();
	}
	else if(dir==-1){
		currentTagID--;
		if(currentTagID < 0 ) currentTagID = tags.size()-1;
	}
		
	drawer.resetTransitions();
}
//--------------------------------------------------------------
void GrafPlayerApp::loadTags()
{
	if( tags.size() < totalToLoad )
	{
		tags.push_back( grafTagMulti() );
		int toLoad = tags.size()-1;
		
		gIO.loadTag( filesToLoad[ toLoad ], &tags[ toLoad ]);
		tags[ toLoad ].tagname = filenames[ toLoad ];
		
		smoother.smoothTag(4, &tags[ toLoad ]);
		tags[toLoad].average();
		tags[toLoad].average();
	}
	else{
		mode = PLAY_MODE_PLAY;
		resetPlayer(0);
	}
	
}
//--------------------------------------------------------------
void GrafPlayerApp::preLoadTags()
{
	filesToLoad.clear();
	filenames.clear();
	totalToLoad = 0;
	
	vector<string> dirs;
	dirLister.setPath( ofToDataPath(myTagDirectory,true) );
	dirLister.findSubDirectories(dirs);
	
	for( int i = 0; i < dirs.size(); i++)
	{
		dirLister.setPath( ofToDataPath(myTagDirectory+dirs[i]+"/",true) );
		dirLister.setExtensionToLookFor("gml");
		int num = dirLister.getNumberOfFiles();
		if( num > 0 )
		{
			filenames.push_back(dirs[i]);
			filesToLoad.push_back(  ofToDataPath(myTagDirectory+dirs[i]+"/"+dirs[i]+".gml") );
		}
	}
	
	
	dirs.clear();
	dirLister.setPath( ofToDataPath(myTagDirectory,true) );
	dirLister.setExtensionToLookFor( "gml" );
	dirLister.getFileNames(dirs);
	
	for( int i = 0; i < dirs.size(); i++)
	{
		filesToLoad.push_back( ofToDataPath(myTagDirectory+dirs[i]) );
		
		string name = dirs[i];
		int endgml = name.find(".gml");
		if( endgml >= 0 )
			name.erase( endgml,name.size() );
		
		filenames.push_back(name);
	}
	
	totalToLoad = filesToLoad.size();
}
//--------------------------------------------------------------
void GrafPlayerApp::setupContolPanel()
{
	
	panel.setup("GA 2.0: Audio", ofGetWidth()-320, 20, 300, 700);
	panel.addPanel("App Settings", 1, false);
	panel.addPanel("Draw Settings", 1, false);
	panel.addPanel("Audio Settings", 1, false);
	
	//---- application sttings
	panel.setWhichPanel("App Settings");
	panel.addToggle("Play / Pause", "PLAY", true);
	panel.addToggle("Rotate", "ROTATE", true);
	panel.addToggle("Display filename", "SHOW_NAME", true);
	panel.addToggle("Display time", "SHOW_TIME", true);
	panel.addToggle("FullScreen", "FULL_SCREEN", false);
	panel.addToggle("Use Fog", "USE_FOG", false);
	panel.addSlider("Fog Start","FOG_START",fogStart,-2000,2000,true);
	panel.addSlider("Fog End","FOG_END",fogEnd,-2000,2000,true);
	panel.addSlider("Rotation Speed","ROT_SPEED",.65,0,4,false);

	//--- draw settings
	panel.setWhichPanel("Draw Settings");
	panel.addSlider("Line Alpha","LINE_ALPHA",.92,0,1,false);
	panel.addSlider("Outline Width","LINE_WIDTH",2,1,10,false);
	panel.addSlider("Line Scale","LINE_SCALE",.05,0,.1,false);
	panel.addSlider("Particle Size","P_SIZE",2,0,10,false);
	panel.addSlider("Particle Alpha","P_ALPHA",.75,0,1,false);
	panel.addSlider("Particle Damping","P_DAMP",.15,0,.25,false);
	panel.addSlider("Number Particles","P_NUM",1,0,4,true);
	panel.addToggle("Use gravity", "USE_GRAVITY", true);
	panel.addToggle("Use edge mask", "USE_MASK", false);
	
	//--- audio settings
	panel.setWhichPanel("Audio Settings");
	vector<string> names_audio_options;
	names_audio_options.push_back("line in");
	names_audio_options.push_back("music file");
	panel.addMultiToggle("audio input:", "audio_input", 1, names_audio_options);
	panel.addToggle("open sound file", "open_sound_file", false);
	panel.addSlider("outward amp force","outward_amp_force",8,0,200,false);
	panel.addSlider("particle size force","particle_size_force",22,0,200,false);
	panel.addSlider("wave deform force","wave_deform_force",.25,0,2,false);
	panel.addSlider("line width force","line_width_force",.25,0,2,false);
	panel.addSlider("change wait time","wait_time",30,0,120,true);
	//panel.addSlider("particle speed force","p_audio_damp",1,0,4,false);

	// toggles to apply what to what...
	panel.addToggle("use particle amp", "use_p_amp", true);
	panel.addToggle("use particle size", "use_p_size", true);
	//panel.addToggle("use particle speed", "use_p_damp", false);
	panel.addToggle("use wave deform", "use_wave_deform", true);
	panel.addToggle("use line width amp", "use_line_width", false);
	panel.addToggle("use bounce", "use_bounce", true);
	
	//--- load saved
	panel.loadSettings("settings/appSettings.xml");
}

//--------------------------------------------------------------
void GrafPlayerApp::updateConrolPanel()
{
	panel.update();
	
	myTagPlayer.bPaused = !panel.getValueB("PLAY");
	bRotating = panel.getValueB("ROTATE");
	bShowName = panel.getValueB("SHOW_NAME");
	bShowTime = panel.getValueB("SHOW_TIME");
	
	if( panel.getValueB("FULL_SCREEN") )
	{
		panel.setValueB("FULL_SCREEN",false);
		ofToggleFullscreen();
	}
	
	bUseFog = panel.getValueB("USE_FOG");
	fogStart = panel.getValueI("FOG_START");
	fogEnd = panel.getValueI("FOG_END");
	
	drawer.setAlpha(panel.getValueF("LINE_ALPHA"));
	drawer.lineWidth = panel.getValueF("LINE_WIDTH");
	
	drawer.setLineScale( panel.getValueF("LINE_SCALE") );
	
	particleDrawer.setParticleSize( panel.getValueF("P_SIZE") );
	//particleDrawer.setDamping( panel.getValueF("P_DAMP") );
	particleDrawer.particle_alpha = panel.getValueF("P_ALPHA") ;
	particleDrawer.numXtras = panel.getValueI("P_NUM");
	bUseGravity = panel.getValueB("USE_GRAVITY");
	bUseMask = panel.getValueB("USE_MASK");
	
	
	//open audio files
	if(panel.getSelectedPanelName() == "Audio Settings" )
	{
	
			if( panel.getValueB("open_sound_file") )
			{
					panel.setValueB("open_sound_file", false); 
					
					char msg[] = {"please select a file"};
					char msg2[] = {""};
					
					string result = dialog.getStringFromDialog(kDialogFile, msg, msg2);
					
					
					resultString.clear();
					resultString.push_back("attempting to load "+result);
					
					vector <string> checkDot = ofSplitString(result, ".");
					
					if(  checkDot.back() == "mp3" || checkDot.back() != "wav"  )
					{
						//resultString.push_back("error - incorrect file type");
						vector <string> paths = ofSplitString(result, "/");
						if( paths.size() )
						{
							resultString.push_back("loading file");
							string currentFilename = paths.back();
							cout << "load: " << currentFilename << endl;
							cout << "result ? " << result << endl;
							//audio.music.loadSound(result);
							//audio.music.play();
						}
						
					}
			}
	}
	
	
	
}

string GrafPlayerApp::getCurrentTagName()
{
	if(tags.size() <= 0 ) return " ";
	else if( mode == PLAY_MODE_LOAD) return tags[tags.size()-1].tagname;
	else return tags[currentTagID].tagname;
}

void GrafPlayerApp::drawAudioLine()
{
	if(tags.size() <= 0 ) return;
	
	// get vector of z for each pt in tag
	vector<float> zDepths;
	
	for( int i = 0; i < tags[currentTagID].myStrokes.size(); i++)
	{
		for( int j = 0; j < tags[currentTagID].myStrokes[i].pts.size(); j++)
		{
			zDepths.push_back( tags[currentTagID ].myStrokes[i].pts[j].pos.z );
		}
		
	}
	
	//cout << "tag center: " << tags[currentTagID].center.x << " " << tags[currentTagID].center.y << endl;
	
	glPushMatrix();
		glTranslatef(tags[currentTagID].min.x,tags[currentTagID].min.y,tags[currentTagID].min.z);
		glTranslatef(tags[currentTagID].center.x,tags[currentTagID].center.y,0);
		glBegin(GL_LINE_STRIP);
		for( int i = 0; i < zDepths.size(); i++)
		{
			int ps = i % NUM_BANDS;
			float bandH = audio.ifftOutput[ps];//audioInput[ps];
			//glVertex3f(0,0,zDepths[i]);
			glVertex3f(0,bandH,zDepths[i]);
		}
		glEnd();
	glPopMatrix();
}

