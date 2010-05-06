#ifndef GRAFEDITOR_H
#define GRAFEDITOR_H

#include "ofMain.h"
#include "guiSlider.h"
#include "guiButton.h"
#include "pocoDirectoryLister.h"

class grafEditor
{
    public:
        grafEditor();
        virtual ~grafEditor();

        void setup(int x, int y, int w, int h);
        bool update(int x, int y);
        void updateMax( int max );
        void draw();

        int     getStart();
        int     getEnd();
        string  getName();
        bool    isEnteringName(){ return bEditName; };

        void    editName( char key );
        void    checkNameForSaving( string directory);
		void	setDefaultName( string defaultName );
		
		guiButton   button_name;
	 string  grafName;

    protected:

        bool    bEditName;

       

        guiSlider slider_start;
        guiSlider slider_end;

        int x,y,w,h;
        int nameCounter;
        string lastName;

        ofxPocoDirectoryLister PL;


    private:
};

#endif // GRAFEDITOR_H
