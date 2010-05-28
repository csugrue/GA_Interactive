#ifndef GRAFVIZPARTICLES_H
#define GRAFVIZPARTICLES_H

#include "ofMain.h"
#include "grafTagMulti.h"
#include "particleSystem.h"

class grafVParticleField
{
    public:
        grafVParticleField();
        virtual ~grafVParticleField();

	
		void setup(int w, int h);
		void reset();
	
		void update( ofPoint pt, ofPoint vel, float dt, bool bReset);
		void fall( float dt);
	
		void setDamping( float val );
		void setParticleSize( float val );
	
		void draw( float zdepth, int screenW, int screenH );
		
		// for particle animations (from audio data)
		void updateParticleSizes(float * vals, float averageVal, int tVals, float maxScale = 4.f);
		void updateDampingFromAudio(float val);
		void updateParticleAmpli(float * vals, float averageVal, int tVals, float maxScale = 4.f);
		
		float alpha;
		float particle_damping;
		float particle_size;
		float particle_alpha;
		
		int numXtras;
	
    protected:
		
		int w, h;
	
		// particles with attached lines
		particleSystem  PS;
	
		// solo particles
		particleSystem XTRA_PS[4];
	
		// normal vector field
		vectorField     VF;
	
		int transitionCounter;
      
    private:
};

#endif // GRAFVIZPARTICLES_H
