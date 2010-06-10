/*
 *  polyFixWidth.h
 *  ensanche_2.0
 *
 *  Created by Chris on 2/28/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */
#pragma once

#include "polyEditable.h"

class polyFixWidth : public polyEditable{
	
public:
	
	polyFixWidth();
	~polyFixWidth();
	polyFixWidth(  const polyFixWidth &  mom );

	void setup();
	void clear();
	void addPoint( ofPoint pt );
	void draw();
	void setWidth( float w );
	void setSquare( bool bSq );
	void setStraight();
	
	ofPoint getCentroid();
		
	vector<ofPoint> rectPts;
	ofPoint center;
	float	length;
	float	width;
	bool	bSquare;
	
	void setFillColor(int r, int g, int b, int a);
	void setStrokeColor(int r, int g, int b, int a);
	
	ofColor fillColor;
	ofColor strokeColor;
	
	// more for debugging
	ofPoint ppA, ppB; // perpendicular points to calculate intersections
	void setPerpendiculars(float len );
	
protected:
	
	void makeRectangle();
	
};

