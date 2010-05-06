#include "grafVParticleField.h"

grafVParticleField::grafVParticleField()
{
    
    alpha				= 1;
    particle_damping	= .12f;
    particle_size		= 2.f;
    particle_alpha		= .75f;
}

grafVParticleField::~grafVParticleField()
{
    //dtor
}

void grafVParticleField::reset()
{
	
    // set up original set with lines
    PS.reset(WIDTH_FIELD,HEIGHT_FIELD);
	
    // set up extras for flare
    for( int i = 0; i < numXtras; i++)
        XTRA_PS[i].reset(WIDTH_FIELD,HEIGHT_FIELD);
	
    // set vector field
    VF.bReset = true;
    VF.clearField();
	
    // reset vars
    alpha = 1;
    transitionCounter = 0;
	numXtras = 1;
}

void grafVParticleField::setup(int w_, int h_)
{
	
    w = w_;
    h = h_;
	
	PS.setup(FIELD_SIZE,WIDTH_FIELD,HEIGHT_FIELD,w,h,&VF);
	
	//particleSystem psx;
	int toatlXtra = 4;
	for( int i = 0; i < toatlXtra; i++)
	{
        //XTRA_PS.push_back(psx);
        XTRA_PS[i].setup(FIELD_SIZE,WIDTH_FIELD,HEIGHT_FIELD,w,h,&VF);
		
	}
	
	setDamping( particle_damping);
	setParticleSize( particle_size );
}

void grafVParticleField::fall(float dt)
{
	for( int j= 0; j< PS.numParticles; j++)
	{
		if( PS.bOn[j] )
		{
			PS.vel[j][1] += (dt) * (40.0);
			PS.pos[j][1] += PS.vel[j][1] * (dt);
		}	 
	 }
	
	for( int i= 0; i< numXtras; i++)
	{
		for( int j= 0; j< XTRA_PS[i].numParticles; j++)
		{
		if( XTRA_PS[i].bOn[j] )
		{
			XTRA_PS[i].vel[j][1] += (dt) * (40.0);
			XTRA_PS[i].pos[j][1] += XTRA_PS[i].vel[j][1] * (dt);
		}	 
		}
	}
}

void grafVParticleField::setDamping( float val )
{
	
    particle_damping = val;
    PS.setParticleDropOffRate(particle_damping);
	
    for( int i = 0; i < numXtras; i++) XTRA_PS[i].dropOffRate = (.8*particle_damping) - (i*.01f );
    //setParticleDropOffRate( (.8*particle_damping) - (i*.01f ));
	
}
void grafVParticleField::setParticleSize( float val )
{
	
    particle_size = val;
    PS.setParticleSize(particle_size);
	
    for( int i = 0; i < numXtras; i++) XTRA_PS[i].setParticleSize(particle_size*.75f);
	
}

void grafVParticleField::update( ofPoint pt, ofPoint vel, float dt, bool bReset)
{
	
	
	
	//if(vel.x == 0 && vel.y == 0 ) 
	float zdepth	= pt.z;
	
    PS.update(dt*100,w,h,zdepth, alpha*particle_alpha, bReset);
	
    for( int i = 0; i < numXtras; i++)
		XTRA_PS[i].update(dt*100,w,h,zdepth, alpha, bReset);
	
    VF.bReset = bReset;
	
	VF.fadeField(0.9f);
	
	vel.x = dt;
	//if(!bReset)
		VF.update( pt, vel, h, w, h);
	
	
}

void grafVParticleField::draw( float zdepth, int screenW, int screenH )
{
    ofNoFill();
	
    glPushMatrix();
	
	for( int i = 0; i <numXtras; i++)
	{
		XTRA_PS[i].draw(zdepth, alpha*particle_alpha, particle_size*.75f, false );
	}
	
	PS.draw(  zdepth, alpha*particle_alpha, particle_size );
	
	
    glPopMatrix();
	
	
	
}