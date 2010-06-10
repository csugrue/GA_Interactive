/*
 *  polyFixWidth.cpp
 *  ensanche_2.0
 *
 *  Created by Chris on 2/28/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#include "polyFixWidth.h"

polyFixWidth::polyFixWidth()
{
	polySimple::polySimple();
	polyAdv::polyAdv();
	polyEditable::polyEditable();
	
	center.set(0,0);
	width = 1;
	bSquare = false;
	length = 0;
	
	ppA.set(0,0);
	ppB.set(0,0);
	
	setFillColor(200,200,200,100);
	setStrokeColor(255,0,0,255);
}

polyFixWidth::~polyFixWidth()
{
	

}

polyFixWidth::polyFixWidth(  const polyFixWidth &  mom ){
	
	polyEditable::polyEditable( (polyEditable)mom);
	
	rectPts.assign(mom.rectPts.begin(),mom.rectPts.end());
	center = mom.center;
	length = mom.length;
	width = mom.width;
	bSquare = mom.bSquare;
	fillColor = mom.fillColor;
	strokeColor = mom.strokeColor;
	ppA = mom.ppA; 
	ppB = mom.ppB;
}

void polyFixWidth::setup()
{
	polyEditable::setup();
}

void polyFixWidth::clear()
{
	polySimple::clear();
	rectPts.clear();
}

void polyFixWidth::addPoint( ofPoint pt )
{
	if( pts.size() < 2)
	{
		pushVertex( pt );
		selectedPoint = pts.size()-1;
		
	}
	
	if( pts.size() == 2 ) makeRectangle();
	
	//cout << "pts added " << pts.size() << endl;
}

void polyFixWidth::draw()
{
	makeRectangle();
	
	ofFill();
	ofSetColor(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
	
	// draw rect
	if( rectPts.size() > 0 )
	{
		ofBeginShape();
		for( int i = 0; i < rectPts.size(); i++)
			ofVertex(rectPts[i].x, rectPts[i].y);
		
		ofVertex(rectPts[0].x, rectPts[0].y);
		ofEndShape();
	}
	
	
	ofNoFill();
	ofSetColor(strokeColor.r, strokeColor.g, strokeColor.b, strokeColor.a);
	
	if( rectPts.size() > 0 )
	{
		ofBeginShape();
		for( int i = 0; i < rectPts.size(); i++)
			ofVertex(rectPts[i].x, rectPts[i].y);
		//ofVertex(rectPts[0].x, rectPts[0].y);
		ofEndShape(true);
	}
	
	// draw end pts
	//polyEditable::draw();
	for( int i = 0; i < pts.size(); i++)
	{
		if( i == selectedPoint && bEnabled ) ofCircle(pts[i].x, pts[i].y, 4);
		if(bEnabled) ofCircle(pts[i].x, pts[i].y, 1);
	}
	
	
	// draw line
	if( pts.size() > 1 && pts.size() < 2 )
	{
		ofSetColor(255,0,0,255);
		//ofLine( pts[0].x, pts[0].y, pts[1].x, pts[1].y);
	}
	
	if( pts.size() == 2 )
	{
		ofPoint c = getCentroid();
		ofCircle(c.x,c.y,2);
	}
	
	
}

void polyFixWidth::setWidth( float w )
{
	width = w;
	makeRectangle();
}

void polyFixWidth::setSquare( bool bSq )
{
	bSquare = bSq;
}

void polyFixWidth::makeRectangle()
{
	if( pts.size() == 2 )
	{
		float hW = width * .5f;
		
		// get perpendicular
		ofxVec2f ptA = ofxVec2f(pts[0].x,pts[0].y);
		ofxVec2f ptB = ofxVec2f(pts[1].x,pts[1].y);
		ofxVec2f ptDiff = ptA-ptB;
		
		center.set((ptA.x+ptB.x)*.5,(ptA.y+ptB.y)*.5);
		
		if( bSquare )
		{
			// hW = length*.5;
			ofxVec2f ptNormal = ptDiff;
			ptNormal = ptNormal.normalize();
			pts[0].set(center.x - hW * ptNormal.x,center.y - hW * ptNormal.y);
			pts[1].set(center.x + hW * ptNormal.x,center.y + hW * ptNormal.y);
			
			ptA.set(pts[0].x,pts[0].y);
			ptB.set(pts[1].x,pts[1].y);
		}
		
		length = ptDiff.length();
		ofxVec2f pp = ptDiff.perpendicular();

		rectPts.clear();
		
		rectPts.push_back( ofPoint(ptA.x + hW * pp.x, ptA.y + hW * pp.y) );
		rectPts.push_back( ofPoint(ptB.x + hW * pp.x, ptB.y + hW * pp.y) );
		rectPts.push_back( ofPoint(ptB.x - hW * pp.x, ptB.y - hW * pp.y) );
		rectPts.push_back( ofPoint(ptA.x - hW * pp.x, ptA.y - hW * pp.y) );
	}else{
		//cout << "t pts " << pts.size() << endl;
	}
	
	
}

/*
void polyFixWidth::mouseDragged(<#ofMouseEventArgs event#>)
{
	cout << "dragged " << endl;
}
*/
ofPoint polyFixWidth::getCentroid()
{
	if(pts.size() == 2){
		center.set((pts[0].x+pts[1].x)*.5,(pts[0].y+pts[1].y)*.5);
		return center;
	}else if( pts.size() > 0 ){
		return pts[0];
	}else
		return ofPoint(0,0);
	
}


void polyFixWidth::setFillColor(int r, int g, int b, int a)
{
	fillColor.r = r;
	fillColor.g = g;
	fillColor.b = b;
	fillColor.a = a;
}


void polyFixWidth::setStrokeColor(int r, int g, int b, int a)
{
	strokeColor.r = r;
	strokeColor.g = g;
	strokeColor.b = b;
	strokeColor.a = a;
}

void polyFixWidth::setStraight()
{
	if( pts.size() == 2 )
	{
		float xDiff = fabs( pts[0].x-pts[1].x );
		float yDiff = fabs( pts[0].y-pts[1].y);
		
		// align to smallest difference
		if( xDiff < yDiff )
		{
			float hDist = xDiff * .5;
			if( pts[0].x > pts[1].x)
			{
				pts[0].x -= hDist;
				pts[1].x += hDist;
			}
			else{
				pts[0].x += hDist;
				pts[1].x -= hDist;

			}
			
		}
		else{
			
			float hDist = yDiff * .5;
			if( pts[0].y > pts[1].y)
			{
				pts[0].y -= hDist;
				pts[1].y += hDist;
			}
			else{
				pts[0].y += hDist;
				pts[1].y -= hDist;
				
			}
			
		}
		
	}
}

void polyFixWidth::setPerpendiculars(float len )
{
	if( pts.size() >= 2 )
	{
		// create vector for each end point 
		ofxVec2f ptA = ofxVec2f(pts[0].x,pts[0].y);
		ofxVec2f ptB = ofxVec2f(pts[1].x,pts[1].y);
		
		// calculate perpendicular
		ofxVec2f diffVec = ptA-ptB;
		
		diffVec = diffVec.getNormalized();
		ofxVec2f pp = diffVec.getPerpendicular();
		
		ofPoint c = getCentroid();
		ppA.set( c.x + len * pp.x, c.y + len * pp.y);
		ppB.set( c.x - len * pp.x, c.y - len * pp.y);
	}
	
}

