#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	
	setupControls();
	
	//grafIo.loadTag("gml/hell-001.gml",&tag);
	
	bUseSmoothing = false;
	
	//int which = gmlLister->selection;
	currentTagName = gmlLister->lister->getName( 0 );
	loadTag( currentTagName);
	
	ofBackground(80,80,80);
	
	GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { 0.0f, 0.0f, 0.0f, 1.0f };
	
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);
	
	
	
}

//--------------------------------------------------------------
void testApp::setupControls()
{
	// set up control panel
	panel.setup("3d model", ofGetWidth()-340, 10, 320, 690);
	
	panel.addPanel("gml", 1, false);
	panel.addPanel("stl", 1, false);
	
	// gml
	panel.setWhichPanel("gml");
	
	lister_gml.listDir("gml");
	gmlLister = panel.addFileLister("gml files", &lister_gml, 250, 100);
	
	vector<string> drawmodes;
	drawmodes.push_back("skeleton");
	drawmodes.push_back("lines");
	drawmodes.push_back("triangles");
	drawmodes.push_back("solid");
	
	panel.addMultiToggle("draw mode", "draw_mode", 2, drawmodes);
	
	panel.addSlider("poly resolution","res",tag.res,0,100,true);
	panel.addSlider("extrude scale","extrude_scale",tag.lineScale,0,100,true);
	panel.addSlider("z const","z_const",tag.z_const,0,100,false);
	panel.addSlider("min extrude width","min_extrude",tag.minLen,0,100,false);
	panel.addSlider("min point dist","min_point_dist",tag.minPtDist,0,.1,false);
	panel.addToggle("use smoothing", "use_smoothing", true);
	panel.addToggle("reload", "reload", false);
	panel.addToggle("default position", "default_position", false);
	//panel.addToggle("set defaults", "set_defaults", false);
	
	// stl
	panel.setWhichPanel("stl");
	
	/*lister_stl.listDir("stl");
	stlLister = panel.addFileLister("stl files", &lister_stl, 400, 100);*/
	
	panel.addTextInput("model name", "model_name", "", 250, 15);
	panel.addToggle("save model","save_model",false);
	
	panel.update();
}

//--------------------------------------------------------------
void testApp::updateControls()
{
	
	//cout << panel.getValueS("model name") << endl;
	
	if( gmlLister->lister->selectedHasChanged() )
	{
		int which = gmlLister->selection;
		currentTagName = gmlLister->lister->getName( which );
		gmlLister->lister->clearChangedFlag();
		
		loadTag( currentTagName );
				
		string newFile = currentTagName;
		int pos = newFile.find_first_of(".gml");
		string mname = newFile.substr(0,pos);
		panel.setValueS("model name",mname);
	}
	
	panel.update();
	
	if(panel.getValueB("save_model"))
	{
		panel.setValueB("save_model",false);
		
		string modelName = "stl/"+panel.getValueS("model name")+".stl";
		cout << "write " << modelName << endl;
		model.write(modelName);
	}
	
	
	
	tag.lineScale = panel.getValueF("extrude_scale");
	tag.z_const = panel.getValueF("z_const");
	tag.minLen = panel.getValueF("min_extrude");
	tag.res = panel.getValueI("res");
	tag.minPtDist = panel.getValueF("min_point_dist");
	
	if(panel.getValueB("reload"))
	{
		panel.setValueB("reload",false);
		
		//string name = tag.tagname+".gml";
		loadTag( currentTagName );

	}
	
	if( panel.getValueB("default_position") )
	{
		panel.setValueB("default_position",false);
		tag.defaultPosition();
	}
	
	
	bUseSmoothing = panel.getValueB( "use_smoothing");
}

//--------------------------------------------------------------
void testApp::update(){

	updateControls();
	tag.bMouseActive = true;
	
}

//--------------------------------------------------------------
void testApp::draw(){

	
	
	if( panel.getSelectedPanelName() == "stl")
	{
		
		glEnable(GL_LIGHTING);
		glPushMatrix();
			glTranslatef(ofGetWidth()/2,ofGetHeight()/2, 0);
			glScalef(700,700,700);
			glRotatef(.25*ofGetFrameNum(), 0,1,0);
			model.draw();
		glPopMatrix();


	}else{
		
		
		//glEnable(GL_LIGHTING);
		glColor4f(1,1,1,.75);
		
		if( panel.getValueI("draw_mode") == 0 ) 
		{
			tag.drawSkeleton();
			tag.drawSpheres();
		}
		else if( panel.getValueI("draw_mode") == 1 ){
			glDisable(GL_LIGHTING);
			glColor4f(1,1,1,.25);
			tag.drawSkeleton();
			tag.drawRadii();
		}
		else if( panel.getValueI("draw_mode") == 2 ){
			glColor4f(1,1,1,.25);
			tag.drawSkeleton();
			glColor4f(1,1,1,.5);
			tag.drawTriangles();
		}
		else if( panel.getValueI("draw_mode") == 3 ){
			tag.drawSkeleton();
			glEnable(GL_LIGHTING);
			glColor4f(1,1,1,1);
			tag.drawTriangles(true);
			glDisable(GL_LIGHTING);
		}
		
	}
	
	
	glDisable(GL_LIGHTING);
	panel.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		
		case OF_KEY_UP:		tag.nextPoint(1); break;
		case OF_KEY_DOWN:	tag.nextPoint(-1); break;
			/*case 's': model.write(ofToString(counter++) + ".stl"); break;
		case 'r': 
			exploding = false;
			model.read("torus.stl"); 
			break;
		case ' ': createPoints(); break;*/
	}
	
	if( panel.isAnyTextBoxActive() ){
		if( key == OF_KEY_BACKSPACE) panel.deleteLastChar();
		else{
			panel.addChar(key);
		}
		return;
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
	panel.mouseDragged(x, y, button);
	
	if( panel.isMouseInPanel(x,y) )
	{
		tag.bMouseActive = false;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	panel.mousePressed(x, y, button);
	if( panel.isMouseInPanel(x,y) )
	{
		tag.bMouseActive = false;
	}
/*exploding = true;
	
	pts.push_back(ofPoint(x,y,0));
	
	float w = ofGetWidth();
	
	if( pts.size() > 2 )
	{
		int lst = pts.size()-1;
		model.addTriangle(
					0, 0, 1,
					pts[lst-2].x/w,pts[lst-2].y/w,pts[lst-2].z/w,
					pts[lst-1].x/w,pts[lst-1].y/w,pts[lst-1].z/w,
					pts[lst].x/w,pts[lst].y/w,pts[lst].z/w
					);	
		
	}*/
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	panel.mouseReleased();

}

void testApp::setModel()
{
	model.clear();
	
	for( int i = 0; i < tag.triangles.size(); i++)
	{
		model.addTriangle(
		0, 0, 1,
		tag.triangles[i].pts[0].x,tag.triangles[i].pts[0].y,tag.triangles[i].pts[0].z,
		tag.triangles[i].pts[1].x,tag.triangles[i].pts[1].y,tag.triangles[i].pts[1].z,
		tag.triangles[i].pts[2].x,tag.triangles[i].pts[2].y,tag.triangles[i].pts[2].z);
	}
	
	model.normalize(1);
	
}


//--------------------------------------------------------------
void testApp::loadTag(string name)
{
	tag.clear();
	tag.drawScale = 768;
	
	// SMOOTHING
	grafIo.loadTag("gml/"+name,&tag);
	if( bUseSmoothing )
	{
		gSmoother.smoothTag(3, &tag);	// change 3 to 4 for more but dont exceed this. 2 is too small
		tag.average();					// add or remove averging for more smoothings
		tag.average();
	}
	
	
	tag.applyDrawScale();
	tag.calcModelData();
	
	setModel();
	model.center();
	
	int pos = name.find_first_of(".gml");
	if(pos > 0 ) name = name.substr(0,pos);
	panel.setValueS("model name",name);
	
	
	
}

/*
if (i == 0) {
	// dirNormal is a normalized vector between spine points //
	// Reynolds Method [url]http://red3d.com/cwr/steer/gdc99/index.html[/url] 
	ofxVec3f new_forward(dirNormal.x, dirNormal.y, dirNormal.z);
	new_forward.normalize();
	ofxVec3f approx_up(-spine[i].orientation.up.x, -spine[i].orientation.up.y, -spine[i].orientation.up.z);
	approx_up.normalize();
	ofxVec3f new_side = new_forward.crossed(approx_up);
	ofxVec3f new_up = new_forward.crossed(new_side);
	new_up.normalize(); // just in case
	
	spine[i].orientation.up.set(new_up.x, new_up.y, new_up.z);
	spine[i].orientation.forward.set(new_forward.x, new_forward.y, new_forward.z);
	spine[i].orientation.side.set(new_side.x, new_side.y, new_side.z);
	
	//////////////////////////////////////////////////////////////////////////////
}  else if (i < numBodyPts) {
	// Game Programming Jems 2 Approach ////////////////////////////////
	ofxVec3f T1(spine[i-1].loc.x, spine[i-1].loc.y, spine[i-1].loc.z);
	ofxVec3f T2(spine[i].loc.x, spine[i].loc.y, spine[i].loc.z);
	ofxVec3f A = T1.crossed(T2);
	float alpha = T1.angleRad(T2);
	
	ofxVec3f new_forward(dirNormal.x, dirNormal.y, dirNormal.z);
	ofxVec3f approx_up(-spine[i-1].orientation.up.x, -spine[i-1].orientation.up.y, -spine[i-1].orientation.up.z);
	approx_up.normalize();
	approx_up.rotateRad(alpha, A);
	ofxVec3f new_side = new_forward.crossed(approx_up);
	ofxVec3f new_up = new_forward.crossed(new_side);
	
	spine[i].orientation.up.set(new_up.x, new_up.y, new_up.z);
	spine[i].orientation.forward.set(new_forward.x, new_forward.y, new_forward.z);
	spine[i].orientation.side.set(new_side.x, new_side.y, new_side.z);
	
}

*/