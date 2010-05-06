#ifndef GRAF_DRAWER_H
#define GRAF_DRAWER_H

#include "ofMain.h"
#include "grafLineDrawer.h"
#include "grafTagMulti.h"

class grafDrawer
{
    public:
        grafDrawer();
        virtual ~grafDrawer();
		
		void setup(grafTagMulti * myTag, float maxLen );
		void draw( int lastStroke, int lastPoint);
		void drawTimeLine( ofPoint center, float currentTime, float startTime, float z_const, ofTrueTypeFont * font, float scale );
		void drawBoundingBox( ofPoint pmin, ofPoint pmax, ofPoint pcenter );
	
		void transition( float dt, float pct );
		void average( float pct );
		void setAlpha(float a);
		void setLineScale( float val );
	
	
		vector<grafLineDrawer *> lines;
		
		float alpha;
		float minLen;
		float lineWidth;
		float lineAlpha;
		float lineScale;
	
		bool bSetupDrawer;
	
	
	
    protected:
    private:
};

#endif // GRAF_DRAWER_H
