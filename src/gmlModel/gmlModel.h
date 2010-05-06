/*
 *  gmlModel.h
 *  GA_3D_v0.1
 *
 *  Created by Chris on 5/3/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */
#include "grafTagMulti.h"

typedef struct extrudeRing{
	vector<ofPoint> rPoints;
	vector<ofPoint> ePoints;
};

typedef struct tris{
	ofPoint pts[3];
};

typedef struct modelData{
	vector<ofxVec3f> vecs;
	vector<float> radii;
	vector<extrudeRing> rings;
};



class GmlModel : public grafTagMulti
{
    public:
        GmlModel();
		~GmlModel();
		
		void defaultPosition();
		
		// draw as circles with radius = width of line
		void drawRadii();
		
		// draw as triangles
		void drawTriangles(bool bFilled = false);
		
		// draw spheres at points
		void drawSpheres();
		
		//------- caclulations for model
		void calcModelData();
		
		void applyDrawScale();

		//------- editing functions
		void twistEndPoints(int index, bool bRecalculate = true);
		void nextPoint( int dir = 1 );
		void drawEditing();
		
		
		//------- data storage for model info
		vector<modelData> myData;
		vector<tris> triangles;
		float * tripts;
		
		float maxLen, minLen, lineScale;
		
		// resolution of circle at each node
		int res;

		//------- editing point by point
		int currPt;	// current point being edited
		
		
	protected:
			
			
			// perpendiculars at each point
			void calcVectors();
	
			// radius (line width) at each point
			void calcRadius();
			
			// points defining a circle at each node based on radius and pp
			void calcRPoints();
			void calcEndPoints();	// give unique end points for each so twisting can be adjusted
			
			// calcs max distance of all lines in strokes
			void calcMaxLen();
			
			// calcs triangles for stl model
			void calcTriangles();
			
			// scaling for fade in and out of line
			float getScaleForLinePosition(int index, int total );
			
	
};
	