/*
 *  gmlModel.cpp
 *  GA_3D_v0.1
 *
 *  Created by Chris on 5/3/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "gmlModel.h"

GmlModel::GmlModel()
{
	grafTagMulti::grafTagMulti();
	
	res			= 10;
	maxLen		= .01;
	minLen		= 1;
	lineScale	= 30;
	currPt		= 0;
}

GmlModel::~GmlModel()
{
	delete [] tripts;
}

void GmlModel::drawTriangles(bool bFilled )
{
	
	if( !bFilled )glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	/*GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat LightPosition[]= { 0.0f, 0.0f, 0.0f, 1.0f };
	
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);
	glEnable(GL_LIGHT1);*/
	//glEnable(GL_LIGHTING);


	//glColor4f(1,1,1,.25);
	
	ofEnableAlphaBlending();
	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
	
	glTranslatef(position.x,position.y,position.z);
	
	// center on screen
	glTranslatef(ofGetWidth()*.5f, ofGetHeight()*.5, 0);	
	

	// rotate
	glRotatef(rotation.x,0,1,0);
	glRotatef(rotation.y,1,0,0);
	glRotatef(rotation.z,0,0,1); 
	
	// center tag
	glTranslatef(-min.x*drawScale,-min.y*drawScale,-min.z);
	glTranslatef(-center.x*drawScale,-center.y*drawScale,-center.z);
	
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	
		glVertexPointer(3, GL_FLOAT, 0, tripts);
		glDrawArrays(GL_TRIANGLES, 0, triangles.size()*3);
	
	
		glDisableClientState(GL_VERTEX_ARRAY);
	
	glPopMatrix();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void GmlModel::drawRadii()
{
	ofEnableAlphaBlending();
	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
		
		glTranslatef(position.x,position.y,position.z);

		// center on screen
		glTranslatef(ofGetWidth()*.5f, ofGetHeight()*.5, 0);	
				
		// rotate
		glRotatef(rotation.x,0,1,0);
		glRotatef(rotation.y,1,0,0);
		glRotatef(rotation.z,0,0,1); 
		
		// center tag
		glTranslatef(-min.x*drawScale,-min.y*drawScale,-min.z);
		glTranslatef(-center.x*drawScale,-center.y*drawScale,-center.z);

		//cout << "pos z " << position.z << endl;

		// draw circles and connecting lines
		for( int i = 0; i < myData.size(); i++)
		{
			for( int j = 0; j < myData[i].rings.size()-1; j++)
			{
					glBegin(GL_LINES);
					ofNoFill();
					glColor4f(1,1,1,.5);
					for( int k = 0; k < myData[i].rings[j].rPoints.size(); k++)
					{
						if( k == 0 )	glColor4f(1,0,0,1);
						else			glColor4f(1,1,1,.5);
						
						glVertex3f(myData[i].rings[j].ePoints[k].x, myData[i].rings[j].ePoints[k].y,myData[i].rings[j].ePoints[k].z);
						glVertex3f(myData[i].rings[j].rPoints[k].x, myData[i].rings[j].rPoints[k].y,myData[i].rings[j].rPoints[k].z);
						/*if( j > 1 )
						{
							glVertex3f(myData[i].rings[j-1].rPoints[k].x, myData[i].rings[j-1].rPoints[k].y,myData[i].rings[j-1].rPoints[k].z);
							glVertex3f(myData[i].rings[j].rPoints[k].x, myData[i].rings[j].rPoints[k].y,myData[i].rings[j].rPoints[k].z);
						}*/
					}
					glEnd();						
					
					ofFill();
					glColor4f(0,0,0,1);
					for( int k = 0; k < myData[i].rings[j].rPoints.size(); k++)
					{
						glPushMatrix();
							glTranslatef(myData[i].rings[j].rPoints[k].x, myData[i].rings[j].rPoints[k].y,myData[i].rings[j].rPoints[k].z);
							ofCircle(0, 0, 1);
						glPopMatrix();
						
					}
					
			}
		}
		
	
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);

	
}

void GmlModel::drawSpheres()
{
		
	glPushMatrix();
	
	glTranslatef(position.x,position.y,position.z);
	
	// center on screen
	glTranslatef(ofGetWidth()*.5f, ofGetHeight()*.5, 0);	
	
	// rotate
	glRotatef(rotation.x,0,1,0);
	glRotatef(rotation.y,1,0,0);
	glRotatef(rotation.z,0,0,1); 
	
	// center tag
	glTranslatef(-min.x*drawScale,-min.y*drawScale,-min.z);
	glTranslatef(-center.x*drawScale,-center.y*drawScale,-center.z);
		
	for( int i = 0; i < myStrokes.size(); i++)
	{
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			if(j==currPt) ofSetColor(255,0,0,255);
			else ofSetColor(255,255,255,255);
			
			glPushMatrix();
				glTranslatef(myStrokes[i].pts[j].pos.x,myStrokes[i].pts[j].pos.y,myStrokes[i].pts[j].pos.z);
				glutSolidSphere(2,8,8);
			glPopMatrix();
			
		}
	}
	
	glPopMatrix();

			
}

void GmlModel::calcModelData()
{
	
	myData.clear();
	
	for( int i = 0; i < myStrokes.size(); i++)
	{
		modelData m;
		myData.push_back(m);
	}
	
	calcMaxLen();		// calculate min and max values for rendering nicely
	calcVectors();		// calculate normal vectors from one point to next	
	calcRadius();		// calculate radius at each point
	calcRPoints();		// calculate points defining a circle at each point, rotate in direction of next to create extrusion
	calcEndPoints();
	calcTriangles();
}

void GmlModel::calcMaxLen()
{
	for( int i = 0; i < myStrokes.size(); i++)
	{
		
		for( int j = 0; j < myStrokes[i].pts.size()-1; j++)
		{
			float dist = myStrokes[i].pts[j].dist;
			if( (i == 0 && j == 0) || dist > maxLen )
			{
				maxLen = dist;
			}
			
		}
	}
	
}

void GmlModel::calcVectors()
{
	if( myStrokes.size() <= 0 || myStrokes[0].pts.size() <= 0 ) return;
	
	// loop thru each stroke and calc vector between each point
	for( int i = 0; i < myStrokes.size(); i++)
	{
		if( myStrokes[i].pts.size() == 0 ) continue;
		
		int numPts = myStrokes[i].pts.size();
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			
			ofxVec3f dirNormal, p1,p2;
			
			if (j > 0 && j < numPts - 1) {
				p1 = ofxVec3f( myStrokes[i].pts[j-1].pos.x, myStrokes[i].pts[j-1].pos.y, myStrokes[i].pts[j-1].pos.z);
				p2 = ofxVec3f( myStrokes[i].pts[j+1].pos.x, myStrokes[i].pts[j+1].pos.y, myStrokes[i].pts[j+1].pos.z);
			} else if (j == numPts - 1) {
				p1 = ofxVec3f( myStrokes[i].pts[j-1].pos.x, myStrokes[i].pts[j-1].pos.y, myStrokes[i].pts[j-1].pos.z);
				p2 = ofxVec3f( myStrokes[i].pts[j].pos.x, myStrokes[i].pts[j].pos.y, myStrokes[i].pts[j].pos.z);
			} else if (j == 0) {
				p1 = ofxVec3f( myStrokes[i].pts[j].pos.x, myStrokes[i].pts[j].pos.y, myStrokes[i].pts[j].pos.z);
				p2 = ofxVec3f( myStrokes[i].pts[j+1].pos.x, myStrokes[i].pts[j+1].pos.y, myStrokes[i].pts[j+1].pos.z);
			}
			
			dirNormal = p2-p1;
			dirNormal = dirNormal.normalize();
			myData[i].vecs.push_back(dirNormal);
		
		}
	}
	
}

void GmlModel::calcRadius()
{
	 // for each point calculate the radius at that point
	for( int i = 0; i < myStrokes.size(); i++)
	{
		myData[i].radii.clear();
		
		if( myStrokes[i].pts.size() == 0 ) continue;
		
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			// calculate the percentage this dist is compared to maxLen of the stroke
			float pct = (myStrokes[i].pts[j].dist) / maxLen;
			
			// make it the inverse so long distances are fatter and short are skinny (pow for more drama)
			pct = powf(1-pct,1.5);
			
			// use linescale to apply a scale to radii 
			// getScaleForLinePosition() applies a fade in and out for ends
			float cslen = (lineScale * pct) * getScaleForLinePosition( j, myStrokes[i].pts.size() );
			
			// keep it at a minimum length  so we have no widths of 0 or too small
			//cout << "len " << cslen << " minlen " << minLen << endl;
			if( cslen < minLen ) cslen = minLen;
			
			// strore radius
			myData[i].radii.push_back( (.5*cslen) );
		}
	
	}
	

}

void GmlModel::calcRPoints()
{
	float angleStep = TWO_PI / (float) res;
	
	// for each point calculate the radius at that point
	for( int i = 0; i < myStrokes.size(); i++)
	{
		if( myStrokes[i].pts.size() == 0 ) continue;
		
		int nPts =  myStrokes[i].pts.size();
		
		ofxVec3f a_up = ofxVec3f(0, -1, 0);
		ofxVec3f n_up = ofxVec3f(0, -1, 0);
		
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
			// new ring
			extrudeRing ring;
			myData[i].rings.push_back(ring);
			
			// length of radius should be calculated already!!!
			float csalen = myData[i].radii[j];
			//if( csalen < minLen ) csalen = minLen;
			
			// rotate around the point
			ofxVec3f pivot = ofxVec3f(myStrokes[i].pts[j].pos.x,myStrokes[i].pts[j].pos.y,myStrokes[i].pts[j].pos.z);
						
			// loop through 360 degrees, stepping at each point based on resolution (res)
			for( int k = 0; k < res; k++)
			{
				float myangle = k*angleStep;
						
				ofxVec3f mydist = ofxVec3f(csalen, 0, 0);
				
				ofxVec3f dirNormal =  myData[i].vecs[j];
				dirNormal.normalize();
				
				// http://red3d.com/cwr/steer/gdc99/index.html
				// http://www.openframeworks.cc/forum/viewtopic.php?f=8&t=3383&p=19025#p19025
				
				// calculate new up vectors
				//if( j > 0 )	n_up = a_up;
				
				ofxVec3f new_forward(dirNormal.x, dirNormal.y, dirNormal.z);
				new_forward.normalize();
				
				a_up = ofxVec3f(-n_up.x, -n_up.y, -n_up.z);
				a_up.normalize();
				
				ofxVec3f new_side = new_forward.crossed(a_up);
				n_up = new_forward.crossed(new_side);
				n_up.normalize(); // just in case
				
				ofxVec3f cross = dirNormal.perpendiculared(n_up);
				cross.rotateRad(myangle, dirNormal);
				cross *= mydist.length();
				cross+=pivot;
				myData[i].rings[j].rPoints.push_back( ofPoint(cross.x,cross.y,cross.z) ) ;//pivot.x+len*newPt.x,pivot.y+len*newPt.y,pivot.z+len*newPt.z) );
				
				
			}
			
		}
		
	}
	
			
}

void GmlModel::calcEndPoints()
{
	// loop through all rpoints and make unique end points so that each can twist in its own way ;)
	for( int i = 0; i < myData.size(); i++)
	{
		// triangles are between two rings 
		for( int j = 0; j < myData[i].rings.size()-1; j++)
		{
			myData[i].rings[j].ePoints.clear();
			for( int k = 0; k < myData[i].rings[j].rPoints.size(); k++)
			{
				myData[i].rings[j].ePoints.push_back(myData[i].rings[j+1].rPoints[k]);
			}
		}
	
	}
	
}

void GmlModel::calcTriangles()
{
	if(triangles.size() > 0 ) delete [] tripts;
	triangles.clear();
	
	// go through each point / ring and create triangles to render as a solid
	for( int i = 0; i < myData.size(); i++)
	{
		// triangles are between two rings so go until next to last
		for( int j = 0; j < myData[i].rings.size()-1; j++)
		{
			for( int k = 0; k < myData[i].rings[j].rPoints.size()-1; k++)
			{
				tris tri;
				triangles.push_back(tri);
				int lst = triangles.size()-1;
				
				triangles[lst].pts[0] = ofPoint(myData[i].rings[j].rPoints[k].x,myData[i].rings[j].rPoints[k].y,myData[i].rings[j].rPoints[k].z);
				triangles[lst].pts[1] = ofPoint(myData[i].rings[j].rPoints[k+1].x,myData[i].rings[j].rPoints[k+1].y,myData[i].rings[j].rPoints[k+1].z);
				//triangles[lst].pts[2] = ofPoint(myData[i].rings[j+1].rPoints[k].x,myData[i].rings[j+1].rPoints[k].y,myData[i].rings[j+1].rPoints[k].z);
				triangles[lst].pts[2] = ofPoint(myData[i].rings[j].ePoints[k].x,myData[i].rings[j].ePoints[k].y,myData[i].rings[j].ePoints[k].z);

				triangles.push_back(tri);
				lst = triangles.size()-1;
				
				triangles[lst].pts[0] = ofPoint(myData[i].rings[j].rPoints[k+1].x,myData[i].rings[j].rPoints[k+1].y,myData[i].rings[j].rPoints[k+1].z);
				//triangles[lst].pts[1] = ofPoint(myData[i].rings[j+1].rPoints[k+1].x,myData[i].rings[j+1].rPoints[k+1].y,myData[i].rings[j+1].rPoints[k+1].z);
				//triangles[lst].pts[2] = ofPoint(myData[i].rings[j+1].rPoints[k].x,myData[i].rings[j+1].rPoints[k].y,myData[i].rings[j+1].rPoints[k].z);
				triangles[lst].pts[1] = ofPoint(myData[i].rings[j].ePoints[k+1].x,myData[i].rings[j].ePoints[k+1].y,myData[i].rings[j].ePoints[k+1].z);
				triangles[lst].pts[2] = ofPoint(myData[i].rings[j].ePoints[k].x,myData[i].rings[j].ePoints[k].y,myData[i].rings[j].ePoints[k].z);
				
			}
			
			// last one
			tris tri;
			triangles.push_back(tri);
			int lst = triangles.size()-1;
			int k = myData[i].rings[j].rPoints.size()-1;
			
			triangles[lst].pts[0] = ofPoint(myData[i].rings[j].rPoints[k].x,myData[i].rings[j].rPoints[k].y,myData[i].rings[j].rPoints[k].z);
			triangles[lst].pts[1] = ofPoint(myData[i].rings[j].rPoints[0].x,myData[i].rings[j].rPoints[0].y,myData[i].rings[j].rPoints[0].z);
			//triangles[lst].pts[2] = ofPoint(myData[i].rings[j+1].rPoints[k].x,myData[i].rings[j+1].rPoints[k].y,myData[i].rings[j+1].rPoints[k].z);
			triangles[lst].pts[2] = ofPoint(myData[i].rings[j].ePoints[k].x,myData[i].rings[j].ePoints[k].y,myData[i].rings[j].ePoints[k].z);
			

			triangles.push_back(tri);
			lst = triangles.size()-1;
			triangles[lst].pts[0] = ofPoint(myData[i].rings[j].rPoints[0].x,myData[i].rings[j].rPoints[0].y,myData[i].rings[j].rPoints[0].z);
			//triangles[lst].pts[1] = ofPoint(myData[i].rings[j+1].rPoints[0].x,myData[i].rings[j+1].rPoints[0].y,myData[i].rings[j+1].rPoints[0].z);
			//triangles[lst].pts[2] = ofPoint(myData[i].rings[j+1].rPoints[k].x,myData[i].rings[j+1].rPoints[k].y,myData[i].rings[j+1].rPoints[k].z);
			triangles[lst].pts[1] = ofPoint(myData[i].rings[j].ePoints[0].x,myData[i].rings[j].ePoints[0].y,myData[i].rings[j].ePoints[0].z);
			triangles[lst].pts[2] = ofPoint(myData[i].rings[j].ePoints[k].x,myData[i].rings[j].ePoints[k].y,myData[i].rings[j].ePoints[k].z);
			
		}
	}
	
	tripts = new float[ (triangles.size()*3)*3 ];
	int t = 0;
	for( int i = 0; i < triangles.size(); i++)
	{
		tripts[t] = triangles[i].pts[0].x;
		tripts[t+1] = triangles[i].pts[0].y;
		tripts[t+2] = triangles[i].pts[0].z;
		t+=3;
		
		tripts[t] = triangles[i].pts[1].x;
		tripts[t+1] = triangles[i].pts[1].y;
		tripts[t+2] = triangles[i].pts[1].z;
		t+=3;
		
		tripts[t] = triangles[i].pts[2].x;
		tripts[t+1] = triangles[i].pts[2].y;
		tripts[t+2] = triangles[i].pts[2].z;
		t+=3;
		
	}
}

float GmlModel::getScaleForLinePosition(int pos, int total )
{

	float pct = 1;
	
    int min = 10;
    int max = total - min;
	
    if (pos < min ) pct = pos / (float) (min-1);
    if (pos > max ) pct = 1 - ( ( pos-max )  / (float) (min-1) );
	
    return pct;
}

void GmlModel::applyDrawScale()
{
	for( int i = 0; i < myStrokes.size(); i++)
	{
		if( myStrokes[i].pts.size() == 0 ) continue;
		
		for( int j = 0; j < myStrokes[i].pts.size(); j++)
		{
		myStrokes[i].pts[j].pos.x *= drawScale;
		myStrokes[i].pts[j].pos.y *= drawScale;
		}
	}
	
	drawScale = 1;
}

void GmlModel::defaultPosition()
{
	position.set(0,0,1);
	rotation.set(0,0,0);
}

//------- editing functions
void GmlModel::twistEndPoints(int index, bool bRecalculate)
{
	
}

void GmlModel::nextPoint( int dir )
{
	if( myStrokes.size() <= 0 || myStrokes[0].pts.size() <= 0 ) return;
	
	if( dir == 1 ) currPt++;
	else currPt--;
	
	
	int t = myStrokes[0].pts.size();
	if(currPt < 0 ) currPt = t-1;
	
	currPt = currPt % t;
	cout << "currPt: " << currPt << " total pts " << myStrokes[0].pts.size() << endl;
	
}

void GmlModel::drawEditing()
{

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