#include "particleSystem.h"

particleSystem::particleSystem()
{
    memset(pos, 0, sizeof(float)*MAX_PARTICLES*3);
    memset(ori, 0, sizeof(float)*MAX_PARTICLES*3);
    memset(col, 0, sizeof(float)*MAX_PARTICLES*4);
    memset(bOn, 0, sizeof(bool)*MAX_PARTICLES);
    memset(lines,0, sizeof(float)*MAX_PARTICLES*3*2);

    dropOffRate = 0.18f;
    force       = 1;
    psize       = 3.0;

}

particleSystem::~particleSystem()
{

}

void particleSystem::clear()
{

}

void particleSystem::reset(int fw, int fh)
{
    float scalex = w  / (float)fw;
	float scaley = h / (float)fh;


	for (int i = 0; i < fw; i++)
	{

		for (int j = 0; j < fh; j++)
		{
			int p = j * fw + i;
			if( p < numParticles )
			{
                float x = i*scalex;
                float y = j*scaley;

                ori[p][0] = x;
                ori[p][1] = y;
                ori[p][2] = 0;

                pos[p][0] = x;
                pos[p][1] = y;
                pos[p][2] = 0;

                vel[p][0] = 0;
                vel[p][1] = 0;
                vel[p][2] = 0;

                col[p][0] = 0;
                col[p][1] = 0;
                col[p][2] = 0;
                col[p][3] = 0;

                bOn[p] = false;
            }

		}
	}
}


void particleSystem::setup(int fieldsize, int fw, int fh, float screenW, float screenH, vectorField * VF)
{
    w = screenW;
    h = screenH;

    numParticles = MIN(fieldsize, MAX_PARTICLES);

    VFptr = VF;

	reset(fw,fh);


}

void particleSystem::update(float dt, float screenW, float screenH, float time, float alpha, bool bReset )
{
    int     fieldPosX,fieldPosY,posp;
    ofPoint addV;
    float   addVX,addVY;

    float damp      = dropOffRate*dt;
    float myalpha   = .75*alpha;

    int totalOn = 0;
    for (int i = 0; i < numParticles; i++)
	{
        if( bOn[i] ){totalOn++; continue;}
        else if( pos[i][0] != ori[i][0] && pos[i][1] != ori[i][1] )
        {
            pos[i][2] = time;
            bOn[i]    = true;
        }
	}


    for (int i = 0; i < numParticles; i++)
	{
        pos[i][0] += vel[i][0]*dt;
        pos[i][1] += vel[i][1]*dt;
	}

	if(!bReset)
	{

	    for (int i = 0; i < numParticles; i++)
        {

            fieldPosX = (int)((pos[i][0]/screenW) * (float)WIDTH_FIELD);
            fieldPosY = (int)((pos[i][1]/screenH) * (float)HEIGHT_FIELD);


            if( fieldPosX < 0 || fieldPosX > WIDTH_FIELD-1 ||
                fieldPosY < 0 || fieldPosY > HEIGHT_FIELD-1
            )
            ;
            else{

                posp = fieldPosY * WIDTH_FIELD + fieldPosX;
                //addV = VFptr->getVel(posp);

                addVX = dt*(force*VFptr->fx[posp]);//addV.x);
                addVY = dt*(force*VFptr->fy[posp]);

                vel[i][0] += addVX;
                vel[i][1] += addVY;

            }
        }


	}



	for (int i = 0; i < numParticles; i++)
	{
        vel[i][0] -= vel[i][0]*damp;
        vel[i][1] -= vel[i][1]*damp;

	}


}

void particleSystem::setParticleForce(float val)
{
    force = val;
}

void particleSystem::setParticleDropOffRate(float val)
{
    dropOffRate = val;
}

void particleSystem::setParticleSize(float val)
{
    psize = val;
}

void particleSystem::draw(float time, float alpha, float size,  bool bDrawLines)
{


    for (int i = 0; i < numParticles; i++)
	{
        if(!bOn[i]){
			col[i][3] = 0;
        }else{ 
			col[i][0] = 1;
			col[i][1] = 1;
			col[i][2] = 1;
			col[i][3] = alpha;
		}
	}

	float linePoints[2][3];
    float lineCol[2][4];

    if(bDrawLines)
    {

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        for (int i = 0; i < numParticles; i++)
		{
		    if(!bOn[i]) continue;

		    linePoints[0][0] = pos[i][0];
            linePoints[0][1] = pos[i][1];
            linePoints[0][2] = pos[i][2];
            linePoints[1][0] = ori[i][0];
            linePoints[1][1] = ori[i][1];
            linePoints[1][2] = pos[i][2];

            lineCol[0][0] = .7;
            lineCol[0][1] = .7;
            lineCol[0][2] = .7;
            lineCol[0][3] = .95*alpha;

            lineCol[1][0] = 0;
            lineCol[1][1] = 0;
            lineCol[1][2] = 0;
            lineCol[1][3] = 0;


            glVertexPointer(3, GL_FLOAT, 0, &linePoints[0]);
            glColorPointer(4, GL_FLOAT, 0, lineCol);
            glDrawArrays(GL_LINES, 0, 2);

		}
		glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }



   /* glPointSize(3);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, pos);
    glColorPointer(4, GL_FLOAT, 0, col);

    glDrawArrays(GL_POINTS, 0, numParticles);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);*/

	float rectPoints[12];
	int w = size*2;
	int h = size*2;
	
    
	glColor4f(1,1,1,alpha);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	
	for (int i = 0; i < numParticles; i++)
	{
		if(col[i][3] <= 0 ) continue;
		
		rectPoints[0] = pos[i][0]-w/2;
		rectPoints[1] = pos[i][1]-h/2;
		rectPoints[2] = pos[i][2];
		
		rectPoints[3] = pos[i][0]+w/2;
		rectPoints[4] = pos[i][1]-h/2;
		rectPoints[5] = pos[i][2];
		
		rectPoints[6] = pos[i][0]+w/2;
		rectPoints[7] = pos[i][1]+h/2;
		rectPoints[8] = pos[i][2];
		
		rectPoints[9] = pos[i][0]-w/2;
		rectPoints[10] = pos[i][1]+h/2;
		rectPoints[11] = pos[i][2];
		
		
		glVertexPointer(3, GL_FLOAT, 0, &rectPoints[0]);
		glDrawArrays( GL_TRIANGLE_FAN, 0, 4);
	}


}
