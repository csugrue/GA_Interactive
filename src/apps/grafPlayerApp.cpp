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
	bUseAudio		= true;
	bUseArchitecture= true;
	
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
	
	// controls
	setupControlPanel();
	updateControlPanel();
	
	// load tgs
	preLoadTags();
	particleDrawer.setup(screenW,screenH);
	
	
	// audio
	if(bUseAudio) audio.setup();
	
	// interactive architecture setup
	if(bUseArchitecture)
	{
		archPhysics.setup();
	}
	
	// temp
	panel.setSelectedPanel("App Settings");
	
	// fbo
	fbo.allocate(FBO_W,FBO_H );
	pWarper.initWarp( FBO_W,FBO_H,FBO_W*.1,FBO_H*.1 );
	pWarper.recalculateWarp();

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
			myTagPlayer.update(&tags[currentTagID]);	// normal play, update tag
		
		}else if( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && waitTimer > 0)			   
		{
			waitTimer -= dt;	// pause time after drawn, before fades out
		}
		else if ( !myTagPlayer.bPaused && myTagPlayer.bDonePlaying && (drawer.alpha > 0 || particleDrawer.alpha > 0))			  		
		{
			updateTransition(0);
		}
		else if (  !myTagPlayer.bPaused && myTagPlayer.bDonePlaying )							
		{
			resetPlayer(1);	// setup for next tag
		}
	
		
		//---------- AUDIO applied
		if( bUseAudio) updateAudio();
		
		
		//--------- ARCHITECTURE
		if( bUseArchitecture ) updateArchitecture();
		
		
		//--------- PARTICLES
		updateParticles();
			
		
		//--------- TAG ROTATION + POSITION
		if(bRotating && !myTagPlayer.bPaused ) rotationY += panel.getValueF("ROT_SPEED")*dt;
		
		// update pos / vel
		tags[currentTagID].position.x += tagPosVel.x;
		tags[currentTagID].position.y += tagPosVel.y;
		
		tagPosVel.x -= .1*tagPosVel.x;
		tagPosVel.y -= .1*tagPosVel.y;

	}
	
	
	
	// controls
	if( bShowPanel ) updateControlPanel();
	
	
}

void GrafPlayerApp::updateParticles()
{
	
	int lastStroke = myTagPlayer.getCurrentStroke();
	int lastPoint  = myTagPlayer.getCurrentId();
	
	if( prevStroke != lastStroke )	myTagPlayer.bReset = true; 
	if( lastPoint <= 0 )			myTagPlayer.bReset = true;
	if( tags[currentTagID].myStrokes[ lastStroke].pts.size()-1 == lastPoint ) myTagPlayer.bReset = true;
	
	particleDrawer.update( myTagPlayer.getCurrentPoint(),myTagPlayer.getVelocityForTime(&tags[currentTagID]),  dt,  myTagPlayer.bReset);
	
	myTagPlayer.bReset = false; // important so no particle error on first stroke
	prevStroke		= myTagPlayer.getCurrentStroke();	
}

void GrafPlayerApp::updateTransition( int type)
{
	//----------  TRANSITIONS
	// fade away, dissolve, deform etc.
	
	drawer.prelimTransTime += dt;
	
	if(bUseAudio)
	{
	
		float deform_frc = panel.getValueF("wave_deform_force");
		float line_amp_frc = panel.getValueF("line_width_force");
		float bounce_frc = panel.getValueF("bounce_force");
		
		if( panel.getValueB("use_wave_deform") ) drawer.transitionDeform( dt,deform_frc, audio.audioInput, NUM_BANDS);
		if( panel.getValueB("use_line_width") ) drawer.transitionLineWidth( dt,audio.averageVal*line_amp_frc);
		if( panel.getValueB("use_bounce") ) drawer.transitionBounce( dt,audio.averageVal*bounce_frc);
		if( drawer.pctTransLine < .1 ) drawer.pctTransLine += .001;
		
	}
	
	
	
	if(bUseArchitecture)
	{
	 
		 if( drawer.prelimTransTime > panel.getValueI("wait_time") )
		 {
			
			bRotating = false;
			
			drawer.transitionFlatten( tags[currentTagID].center.z, 50);
			particleDrawer.flatten( tags[currentTagID].center.z, 52);
			
			if(rotFixTime == 0) rotFixTime = ofGetElapsedTimef();
			float pct = 1 - ((ofGetElapsedTimef()-rotFixTime) / 45.f);
			rotationY = pct*rotationY + (1-pct)*(0);
			
			if( pct < .9 && !archPhysics.bMakingParticles) 
				archPhysics.turnOnParticleBoxes(&particleDrawer.PS);

			if(particleDrawer.xalpha  > 0 ) particleDrawer.xalpha -= .5*dt;
			
			// if all particles have fallen and 
			if(archPhysics.bMadeAll)
			{
				// do average transition
				//drawer.transition(dt,.99);
				drawer.alpha -= .1*dt;
				if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
			}
			
			//if( bUseGravity ) particleDrawer.fall(dt);
			
		 }
	
	}else{
	
		// do average transition
		//drawer.transition(dt,.99);
		if(!bUseAudio)
		{
			drawer.alpha -= .1*dt;
			drawer.transition(dt,.15);
			if( bUseGravity ) particleDrawer.fall(dt);
			if(particleDrawer.alpha  > 0 ) particleDrawer.alpha -= .5*dt;
		}
	}
	
	//---------- 
}

void GrafPlayerApp::updateAudio()
{
	if(panel.getValueB("use_p_size") ) 
		particleDrawer.updateParticleSizes(audio.eqOutput,audio.averageVal, NUM_BANDS,panel.getValueF("particle_size_force") );
	
	particleDrawer.setDamping( panel.getValueF("P_DAMP") );
	
	if( /*drawer.prelimTransTime < panel.getValueI("wait_time")  &&*/ panel.getValueB("use_p_amp") ) 
		particleDrawer.updateParticleAmpli(audio.ifftOutput,audio.averageVal, NUM_BANDS,panel.getValueF("outward_amp_force") );
	
	// create drops
	for( int i = 0; i < audio.peakFades.size(); i++)
	{
		if( audio.peakFades[i] == 1 )
		{
			int randomP = particleDrawer.PS.getIndexOfRandomAliveParticle();//ofRandom( 0, particleDrawer.PS.numParticles );
			ofPoint pPos = ofPoint(particleDrawer.PS.pos[randomP][0],particleDrawer.PS.pos[randomP][1],particleDrawer.PS.pos[randomP][2]);
			ofPoint pVel = ofPoint(particleDrawer.PS.vel[randomP][0],particleDrawer.PS.vel[randomP][1],particleDrawer.PS.vel[randomP][2]);
			drops.createRandomDrop( pPos, pVel, particleDrawer.PS.sizes[randomP] );
		}
	}
	
	// update particle drops (audio stuff);
	drops.update(dt);
	
	// update audio
	audio.update();
}

void GrafPlayerApp::updateArchitecture()
{
	//if( drawer.prelimTransTime < panel.getValueI("wait_time") && !archPhysics.bMakingParticles)
	//	archPhysics.turnOnParticleBoxes(&particleDrawer.PS);
	
	archPhysics.update(dt);
	if(archPhysics.bMakingParticles)
	{
		archPhysics.createParticleSet(&particleDrawer.PS);
	}
	
}

//--------------------------------------------------------------
void GrafPlayerApp::draw(){

	screenW = 1024;//ofGetWidth();
	screenH = 768;//ofGetHeight();
	
	
	
	// architecture test image
	if( mode == PLAY_MODE_PLAY )
	{
		ofSetColor(150,150,150,255);
		if( bUseArchitecture && panel.getValueB("show_image") )
			archPhysics.drawTestImage();
	}
	
	
	
	//--------- start fbo render
	fbo.clear();
	fbo.begin();
	
	ofEnableAlphaBlending();
	ofSetColor(255,255,255,255);

	if( mode == PLAY_MODE_LOAD )
	{
		//nothing whi/e loading
		;
	}
	else if( mode == PLAY_MODE_PLAY )
	{
	
		//adjust viewport to match position of tag
		glViewport(tags[currentTagID].position.x,-tags[currentTagID].position.y,fbo.texData.width,fbo.texData.height);
		
		
		glPushMatrix();
		
			if( bUseFog )
			{
				glFogf(GL_FOG_START, fogStart );
				glFogf(GL_FOG_END, fogEnd );
				glEnable(GL_FOG);				
			}
		
			//glTranslatef(tags[currentTagID].position.x,tags[currentTagID].position.y,0);
			glTranslatef(screenW/2, screenH/2, 0);
			glScalef(tags[currentTagID].position.z,tags[currentTagID].position.z,tags[currentTagID].position.z);//tags[currentTagID].position.z);
		
			glPushMatrix();
		
				glRotatef(tags[currentTagID].rotation.x,1,0,0);
				glRotatef(tags[currentTagID].rotation.y+rotationY,0,1,0);
				glRotatef(tags[currentTagID].rotation.z,0,0,1);
		
				glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
				glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,-tags[currentTagID].center.z);
		
				glDisable(GL_DEPTH_TEST);
								
				// draw particles
				particleDrawer.draw(myTagPlayer.getCurrentPoint().z,  screenW,  screenH);
								
				// draw audio particles
				if( bUseAudio) drops.draw();
				
				glEnable(GL_DEPTH_TEST);
				
				// draw lines
				glPushMatrix();
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					drawer.draw( myTagPlayer.getCurrentStroke(), myTagPlayer.getCurrentId() );
				glPopMatrix();
		
				// draw bounding box
				glPushMatrix();
					ofSetColor(255, 255, 255);
					glScalef( tags[currentTagID].drawScale, tags[currentTagID].drawScale, 1);
					//tags[currentTagID].drawBoundingBox( tags[currentTagID].min, tags[currentTagID].max, tags[currentTagID].center );
				glPopMatrix();
				
				
		
			glPopMatrix();
		
			
		
		glPopMatrix();
		
		
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_FOG);
			
	}
	
	
	if(bUseArchitecture)
	{
		
		glViewport(0,0,fbo.texData.width,fbo.texData.height);
		//
		// set translation in polygon tool so drwaing happens in correct place
		archPhysics.offSetPre.x = (tags[currentTagID].position.x);
		archPhysics.offSetPre.y = (tags[currentTagID].position.y);
		archPhysics.offSet.x = (-tags[currentTagID].min.x*tags[currentTagID].drawScale) + (-tags[currentTagID].center.x*tags[currentTagID].drawScale);
		archPhysics.offSet.y = (-tags[currentTagID].min.y*tags[currentTagID].drawScale) + (-tags[currentTagID].center.y*tags[currentTagID].drawScale);
		archPhysics.scale = tags[currentTagID].position.z;
		
		//archPhysics.pGroup.setOffset(ofPoint(offX, offY,0), ofPoint(tags[currentTagID].position.x,tags[currentTagID].position.y,0));
		
		
		
		glPushMatrix();
			//glTranslatef(screenW/2, screenH/2, 0);
			//glTranslatef(-tags[currentTagID].min.x*tags[currentTagID].drawScale,-tags[currentTagID].min.y*tags[currentTagID].drawScale,-tags[currentTagID].min.z);
			//glTranslatef(-tags[currentTagID].center.x*tags[currentTagID].drawScale,-tags[currentTagID].center.y*tags[currentTagID].drawScale,0);
			archPhysics.draw();
			
			//for( int i = 0; i < wPolys.size(); i++)
			//	wPolys[i].draw();
				
			if( archPhysics.bDrawingActive || panel.getValueB("show_drawing_tool") )
			{
				//archPhysics.drawTool();
			}
			
		glPopMatrix();
	}
	
	
	// image mask for edges
	if(bUseMask)
	{
		ofEnableAlphaBlending();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		imageMask.draw(0,0,ofGetWidth(),ofGetHeight());
	}
	
	// data
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
	
	
	//---- end fbo render
	fbo.end();
	
	if(bUseArchitecture)
	{
		
		//glViewport(0,0,fbo.texData.width,fbo.texData.height);
		
		glPushMatrix();
	
		
		if( archPhysics.bDrawingActive || panel.getValueB("show_drawing_tool") )
		{
			archPhysics.drawTool();
		}
		
		glPopMatrix();
	}
	
	
	ofEnableAlphaBlending();
	
	//---- draw fbo to screen
	ofSetColor(255,255,255,250);
	fbo.drawWarped(0, 0,FBO_W, FBO_H,pWarper.u,pWarper.v,pWarper.num_x_sections,pWarper.num_y_sections);
	//draw(0, 0,FBO_W, FBO_H);
	
	// control panel
	if(bShowPanel){
		
		if( panel.getSelectedPanelName() == "App Settings" )
		{
			pWarper.drawEditInterface(10, 10, .25);
			pWarper.drawUV(0, 0, 1);
		}
		
		panel.draw();
		//fontSS.drawString("x: toggle control panel  |  p: pause/play  |  s: screen capture  |  m: toggle mouse  |  f: toggle fullscreen  |  h: toggle home  |  arrows: next/prev  |  esc: quit", 90, ofGetHeight()-50);
		//fontSS.drawString("left mouse: alter position  |  left+shift mouse: zoom  |  right mouse: rotate y  |  right+shift mouse: rotate x", 220, ofGetHeight()-30);
		
		if( bUseAudio && panel.getSelectedPanelName() == "Audio Settings" )
		{
			audio.draw();
		}
		
		
	}
}


//--------------------------------------------------------------
void GrafPlayerApp::keyPressed (ofKeyEventArgs & event){


	
    switch(event.key){

  		case 'x': bShowPanel=!bShowPanel; break;
		case 'p': 
			myTagPlayer.bPaused = !myTagPlayer.bPaused;
			//bRotating = !myTagPlayer.bPaused;
			panel.setValueB("ROTATE",bRotating);
			panel.setValueB("PLAY",!myTagPlayer.bPaused);
			break;
			
			
		case OF_KEY_RIGHT:  resetPlayer(1); break;
        case OF_KEY_LEFT:   resetPlayer(-1); break;
		
		case 's': bTakeScreenShot = true; break;
			
		case '/': 
			archPhysics.turnOnParticleBoxes(&particleDrawer.PS);
			break;
		case '?' : 	
			cout << "total pts for this tag: " << tags[currentTagID].getNPts() << endl;
			if(drawer.lines.size() >0)archPhysics.turnOnParticlesForLine(drawer.lines[0]->pts_r);
			break;
		case 'R':
			tags[currentTagID].rotation.set(0,0,0);
			tags[currentTagID].position.set(0,0,1);
			rotationY = 0;
			tagPosVel.set(0,0,0);
			break;
		case OF_KEY_RETURN:
			if( panel.getSelectedPanelName() == "Architecture Drawing" )
				archPhysics.pGroup.addPoly();
			break;
		default:
  			break;

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
	
	bool bMoveTag = true;
	
	if( panel.isMouseInPanel(event.x, event.y) )						bMoveTag = false;
	else if( panel.getSelectedPanelName() == "Architecture Drawing")	bMoveTag = false;
	else if( pWarper.isEditing() )										bMoveTag = false;
	
	if( bMoveTag )
	{
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
			if( tags.size() > 0 && !bShiftOn)		tags[currentTagID].rotation.y += (event.x-lastX);
			else if( tags.size() > 0 && bShiftOn)	tags[currentTagID].rotation.x += (event.y-lastY);
		}
	}	
	
	
	lastX   = event.x;
	lastY   = event.y;

}

//--------------------------------------------------------------
void GrafPlayerApp::mousePressed(ofMouseEventArgs & event ){

    if( bShowPanel ) panel.mousePressed(event.x,event.y,event.button);
	
	
	if(bUseArchitecture)
	{
		if( panel.isMouseInPanel(event.x, event.y) ) archPhysics.pGroup.disableAll(true);
		else if( panel.getSelectedPanelName() == "Architecture Drawing") archPhysics.pGroup.reEnableLast();
	}
	
	
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
	rotFixTime = 0;
	archPhysics.reset();
	
	cout << "total pts this tag " << tags[currentTagID].getNPts() << endl;
	
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
void GrafPlayerApp::setupControlPanel()
{
	
	panel.setup("GA 2.0: Audio", ofGetWidth()-320, 20, 300, 700);
	panel.addPanel("App Settings", 1, false);
	panel.addPanel("Draw Settings", 1, false);
	panel.addPanel("Audio Settings", 1, false);
	panel.addPanel("Architecture Drawing", 1, false);
	panel.addPanel("Architecture Settings", 1, false);
	panel.addPanel("App Modes", 1, false);

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
	panel.addToggle("Load Warping", "load_warping", false);
	panel.addToggle("Save Warping", "save_warping", false);
	
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
	/*vector<string> names_audio_options;
	names_audio_options.push_back("line in");
	names_audio_options.push_back("music file");
	panel.addMultiToggle("audio input:", "audio_input", 1, names_audio_options);
	*/
	panel.addToggle("open sound file", "open_sound_file", false);
	panel.addSlider("outward amp force","outward_amp_force",8,0,200,false);
	panel.addSlider("particle size force","particle_size_force",22,0,200,false);
	panel.addSlider("wave deform force","wave_deform_force",.25,0,2,false);
	panel.addSlider("line width force","line_width_force",.25,0,2,false);
	panel.addSlider("bounce force","bounce_force",.25,0,2,false);
	panel.addSlider("change wait time","wait_time",30,0,120,true);
	panel.addSlider("drop p threshold","drop_p_thresh",.1,0,2,false);
	
	//panel.addSlider("particle speed force","p_audio_damp",1,0,4,false);

	// toggles to apply what to what...
	panel.addToggle("use particle amp", "use_p_amp", true);
	panel.addToggle("use particle size", "use_p_size", true);
	//panel.addToggle("use particle speed", "use_p_damp", false);
	panel.addToggle("use wave deform", "use_wave_deform", true);
	panel.addToggle("use line width amp", "use_line_width", false);
	panel.addToggle("use bounce", "use_bounce", true);
	
	panel.setWhichPanel("Architecture Settings");
	panel.addToggle("show drawing tool", "show_drawing_tool",false);
	panel.addToggle("show image", "show_image",true);

	panel.setWhichPanel("Architecture Drawing");
	panel.addToggle("new structure", "new_structure",false);
	panel.addToggle("done","arch_done",false);
	panel.addToggle("save xml", "arch_save", false);
	panel.addToggle("load xml", "arch_load", false);
	panel.addToggle("clear", "arch_clear", false);
	
	panel.setWhichPanel("App Modes");
	panel.addToggle("use audio", "use_audio",true);
	panel.addToggle("use architecture", "use_arch",true);

	//--- load saved
	panel.loadSettings("settings/appSettings.xml");
	
	panel.update();
}

//--------------------------------------------------------------
void GrafPlayerApp::updateControlPanel()
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
	particleDrawer.particle_alpha = panel.getValueF("P_ALPHA") ;
	particleDrawer.numXtras = panel.getValueI("P_NUM");
	bUseGravity = panel.getValueB("USE_GRAVITY");
	bUseMask = panel.getValueB("USE_MASK");
	
	audio.peakThreshold = panel.getValueF("drop_p_thresh");
	
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
							audio.music.loadSound(result);
							//audio.music.play();
						}
						
					}
			}
	}
	
	// architecture
	if( panel.bNewPanelSelected && panel.getSelectedPanelName() == "Architecture Drawing")
	{
		archPhysics.bDrawingActive = true;
		archPhysics.pGroup.reEnableLast();
	}
	
	if(panel.getSelectedPanelName() != "Architecture Drawing")
	{
		archPhysics.bDrawingActive = false;
		archPhysics.pGroup.disableAll(true);
	}
	
	if( panel.getValueB("arch_done") )
	{
		panel.setValueB("arch_done",false);
		//archPhysics.createArchitectureFromPolys();
		createWarpedArchitecture();
	}
	
	if( panel.getValueB("new_structure") )
	{
		panel.setValueB("new_structure",false);
		archPhysics.pGroup.addPoly();
	}
	
	if( panel.getValueB("arch_save") )
	{
		panel.setValueB("arch_save",false);
		archPhysics.saveToXML();
	}
	
	if( panel.getValueB("arch_load") )
	{
		panel.setValueB("arch_load",false);
		archPhysics.loadFromXML();
		createWarpedArchitecture();
	}
	
	if( panel.getValueB("arch_clear") )
	{
		panel.setValueB("arch_clear",false);
		archPhysics.pGroup.clear();
		archPhysics.pGroup.addPoly();
	}
	
	archPhysics.bShowArchitecture = panel.getValueB("show_drawing_tool");
	
	bUseAudio = panel.getValueB("use_audio");
	bUseArchitecture = panel.getValueB("use_arch");
	
	if(panel.getSelectedPanelName() == "App Settings")
	{
		pWarper.enableEditing();
	}else
		pWarper.disableEditing();
	
	
	if( panel.getValueB("load_warping") )
	{
		panel.setValueB("load_warping",false);
		pWarper.loadFromXml("settings/warper.xml");
	}
	if( panel.getValueB("save_warping") )
	{
		panel.setValueB("save_warping",false);
		pWarper.saveToXml("settings/warper.xml");
	}
}

string GrafPlayerApp::getCurrentTagName()
{
	if(tags.size() <= 0 ) return " ";
	else if( mode == PLAY_MODE_LOAD) return tags[tags.size()-1].tagname;
	else return tags[currentTagID].tagname;
}


void GrafPlayerApp::createWarpedArchitecture()
{
	wPolys.clear();
	
	for( int i = 0; i < archPhysics.pGroup.polys.size(); i++)
	{
		polySimple tPoly;
		tPoly.pts.assign( archPhysics.pGroup.polys[i]->pts.begin(),archPhysics.pGroup.polys[i]->pts.end() );
		wPolys.push_back(tPoly);
	}
	
	for( int i = 0; i < wPolys.size(); i++)
	{
		for( int j = 0; j < wPolys[i].pts.size(); j++)
		{
			ofPoint wPoint = pWarper.warpPoint(wPolys[i].pts[j]);
			wPolys[i].pts[j] = wPoint;
		}
	}
	
	archPhysics.createArchitectureFromPolys(wPolys);
}




