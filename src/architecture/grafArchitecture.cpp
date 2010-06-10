/*
 *  grafArchitecture.cpp
 *  GA_Interactive
 *
 *  Created by Chris on 6/5/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "grafArchitecture.h"

GrafArchitecture::GrafArchitecture()
{
}

GrafArchitecture::~GrafArchitecture()
{
	
}

void GrafArchitecture::clear()
{
}

void GrafArchitecture::reset()
{
	bMakingParticles	= false;
	bMadeAll			= false;
	bPauseKill			= false;
	totalParticlesMade	= 0;

	for(int i=0; i<boxes.size(); i++)
		boxes[i].destroyShape();
	
	boxes.clear();
}


void GrafArchitecture::setup()
{
	box2d.init();
	box2d.setGravity(0, 10);
	box2d.createFloor();
	box2d.checkBounds(true);
	box2d.setFPS(30.0);
	
	bDrawingActive	= true;
	toolType		= TOOL_TYPE_POLY;
	pGroup.setup();
	pGroup.setStrokeColor(255, 0, 255, 255);
	
	bShowArchitecture = false;
	bMakingParticles = false;
	bMadeAll = false;
	bPauseKill = false;
	
	archImage.loadImage("images/archImage.jpg");
	bUseTestImage	= true;
	
	offSet.set(0,0);
	offSetPre.set(0,0);
	scale = 1;
	// load save architecture
	
	
}

void GrafArchitecture::turnOnParticleBoxes(particleSystem * PS)
{
	reset();
	
	bMakingParticles = true;
	
	for( int i = PS->numParticles-1; i >= 0; i--)
		PS->bDrawOn[i] = true;
		
	// create random sort
	keys.clear();
	vector<int> world;
	int magnitude = PS->numParticles-1;
	
 	for(int i=0;i<PS->numParticles;i++){
		world.push_back(i);
		keys.push_back(0);
	}
	
	for(int i=0;i<PS->numParticles;i++){
		int pos = int(ofRandom(0,magnitude));
		keys[i] = world[pos];
		world[pos] = world[magnitude];
		magnitude--;
	}
	
}

void GrafArchitecture::createParticleSet(particleSystem * PS)
{
	float screenW = 1024;//ofGetWidth();
	float screenH = 768;//ofGetHeight();
	
	int countThisSet = 0;
	int numThisTime = ofRandom(PS_AT_A_TIME*.25,PS_AT_A_TIME);
	
	totalParticlesMade = 0;
	
	for( int i = PS->numParticles-1; i >= 0; i--)
	{
		int me = keys[i];
		
		// if this particles is not turned drawing off, ok to make it
		// if off add to total off
		if(PS->col[me][3] == 0 ) PS->bDrawOn[me] = false;
		
		if( PS->bDrawOn[me] && countThisSet < numThisTime && boxes.size() < 300)
		{
			float size = PS->sizes[me];
			GrafArchBox rect;
			rect.setPhysics(0.4, 0.53, 10.31);
			rect.setup(box2d.getWorld(), (screenW/2)+offSetPre.x+( scale*PS->pos[me][0]+scale*offSet.x), (screenH/2)+offSetPre.y+(scale*PS->pos[me][1]+scale*offSet.y), scale*size, scale*size);
			rect.setAlpha(PS->col[me][3]);
			rect.setRandomLifespan(2,10);
			boxes.push_back(rect);
			
			PS->bDrawOn[me] = false;
			PS->col[me][3] = 0;
			
			countThisSet++;
			
		}else if( !PS->bDrawOn[me] ){
			totalParticlesMade++;
		}
		
	}
	
	if( totalParticlesMade >=  PS->numParticles ) 
	{	
		bMadeAll = true;
		//bPauseKill = true;
	}
	
	
	
	
}

void GrafArchitecture::setParticleBoxes(particleSystem * PS)
{
	float screenW = ofGetWidth();
	float screenH = ofGetHeight();
	
	for(int i=0; i<boxes.size(); i++)
		boxes[i].destroyShape();
	
	boxes.clear();
	
	int tOn  = 0;
	
	/*
	for( int i = PS->numParticles-1; i >= 0; i-=2)
	{
		
		if(  PS->framesOn[i] > 0 && boxes.size() < 500)
		{
		float size = PS->sizes[i];
		GrafArchBox rect;
		rect.setPhysics(3.0, 0.53, 0.1);
		rect.setup(box2d.getWorld(), (screenW/2)+offSetPre.x+( scale*PS->pos[i][0]+scale*offSet.x), (screenH/2)+offSetPre.y+(scale*PS->pos[i][1]+scale*offSet.y), scale*size, scale*size);
		rect.setAlpha(PS->col[i][3]);
		boxes.push_back(rect);
		
		PS->bDrawOn[i] = false;
		PS->col[i][3] = 0;
		
		}else if( PS->framesOn[i] > 0 )
			tOn++;
	}*/
	
	//for( int i = PS->numParticles-1; i >= 0; i--) 
	//	
	//cout << "boxes: " << boxes.size() << "tota/lOn " << tOn << endl;
}

void GrafArchitecture::turnOnParticlesForLine(vector<ofPoint> pts)
{
	float screenW = ofGetWidth();
	float screenH = ofGetHeight();
	
	for(int i=0; i<lineCircles.size(); i++)
		lineCircles[i].destroyShape();
	
	lineCircles.clear();
	
	//return;
	
	/*for( int i = 0; i < lineCircles.size(); i++)
	{
		*/
	
	//if(lineCircles.size() > 0) lineCircles.clear();
	
	//for( int i = 0; i < lines.size(); i++)
	//{
		int total = MIN(30,pts.size() );
		
		for( int j = 0; j < total; j++)
		{
			float x = (screenW/2)+offSetPre.x+(scale*(pts[j].x*768)+scale*offSet.x);
			float y = (screenH/2)+offSetPre.y+(scale*(pts[j].y*768)+scale*offSet.y);
			ofxBox2dCircle circle;
			circle.setPhysics(3.0, 0.53, 0.1);
			circle.setup(box2d.getWorld(), x, y, 2);//,4);
			lineCircles.push_back(circle);
			//circle.destroyShape();//			
		}
	//}
	
}

void GrafArchitecture::update(float dt)
{
	box2d.update();
	
	//if(bDrawingActive)	pGroup.reEnableLast();
	//else				pGroup.disableAll(true);
	
	
	int screenH = ofGetHeight();
	for(int i=boxes.size()-1; i>=0; i--)
	{
		boxes[i].updateAlive(dt);
		
		if(boxes[i].getTimeAlive() > boxes[i].lifeSpanTime && !bPauseKill)
		{
		boxes[i].fadeAlpha(dt);
		}
		
		
		if( boxes[i].getPosition().y > screenH || boxes[i].alpha <= 0)
		{
			boxes[i].destroyShape();
			boxes.erase(boxes.begin()+i);
		}
	}
	
	//cout << "total boxes: " << boxes.size() << endl;
}

void GrafArchitecture::draw()
{
	for(int i=0; i<boxes.size(); i++)
	{
		boxes[i].draw();
	}
	
	for(int i=0; i<lineCircles.size(); i++)
	{
		lineCircles[i].draw();
	}
	
	
	if(bShowArchitecture)
	{
		for(int i=0; i<drawLines.size(); i++)
		{
			drawLines[i].draw();
		}
	}
	
	
}

void GrafArchitecture::drawTool()
{
	
	ofNoFill();
	ofSetColor(255,0,255,255);
	pGroup.draw();
	
}

void GrafArchitecture::drawTestImage()
{
	float scaleImage = (float)ofGetWidth() /  (float)archImage.width;
	archImage.draw(0,0,scaleImage*archImage.width,scaleImage*archImage.height);
}

//--------------------------------------------------------------
void GrafArchitecture::createArchitectureFromPolys()
{
	for( int i = 0; i < drawLines.size(); i++)
		drawLines[i].destroyShape();
	
	drawLines.clear();
	
	for( int i = 0; i < pGroup.polys.size(); i++)
	{
		//drawLines
		ofxBox2dLine lineStrip;
		lineStrip.setWorld(box2d.getWorld());
		lineStrip.clear();
		for(int j=0; j<pGroup.polys[i]->pts.size(); j++)
		{
			lineStrip.addPoint(pGroup.polys[i]->pts[j].x, pGroup.polys[i]->pts[j].y);
		}
		lineStrip.createShape();
		drawLines.push_back(lineStrip);
	}
	
}

void GrafArchitecture::saveToXML()
{
	ofxXmlSettings xml;
	
	xml.addTag("grafArchitecture");
	xml.pushTag("grafArchitecture");
		for( int i = 0; i < pGroup.polys.size(); i++)
		{
			xml.addTag("structure");
			xml.setAttribute("structure","type","line",i);
			xml.setAttribute("structure","bClosed",pGroup.polys[i]->bClosed,i);
			xml.pushTag("structure", i);
				for( int j = 0; j < pGroup.polys[i]->pts.size(); j++)
				{
					xml.addTag("pt");
					xml.setAttribute("pt","x", pGroup.polys[i]->pts[j].x, j);
					xml.setAttribute("pt","y", pGroup.polys[i]->pts[j].y, j);
				}
			xml.popTag();
		}
	xml.popTag();
	
	xml.saveFile("settings/architecture.xml");
}

void GrafArchitecture::loadFromXML()
{
	ofxXmlSettings xml;
	xml.loadFile("settings/architecture.xml");
	
	xml.pushTag("grafArchitecture");
	
	int totalPolys = xml.getNumTags("structure");
	
	if(totalPolys > 0 ) pGroup.clear();
	
	for( int i = 0; i < totalPolys; i++)
	{
		pGroup.addPoly();
		
		//xml.getAttribute("structure","type",i);
		//xml.getAttribute("structure","bClosed",i);
		xml.pushTag("structure", i);
		
		int totalPts = xml.getNumTags("pt");
		cout << "num pts " << totalPts << endl;
		
		for( int j = 0; j < totalPts; j++)
		{
			
			float xpt = xml.getAttribute("pt","x", 0,j);
			float ypt = xml.getAttribute("pt","y", 0,j);
			
			pGroup.addPoint( ofPoint(xpt,ypt), false );
		}
		xml.popTag();
	}
	xml.popTag();
	
	createArchitectureFromPolys();
}
/*
void GrafArchitecture::ofUniqueRandom(vector<int> * indices){
	
	vector<int> world;
	int magnitude = indices.size()-1;
	
 	for(int i=0;i<indices.size();i++){
		world[i] = i;
		keys[i] = 0;
	}
	
	for(int i=0;i<dim;i++){
		int pos = int(ofRandom(0,magnitude));
		keys[i] = world[pos];
		world[pos] = world[magnitude];
		magnitude--;
	}
	return keys;
}*/