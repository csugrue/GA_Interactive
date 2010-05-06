#include "grafEditor.h"

grafEditor::grafEditor()
{
    grafName = "";
    lastName = "";
    bEditName = false;
    nameCounter = 0;
}

grafEditor::~grafEditor()
{
    //dtor
}

void grafEditor::setup(int x_, int y_, int w_, int h_)
{
    x = x_;
    y = y_;
    w = w_;
    h = h_;

    slider_end.setup( "start",0,0,1,x,y,w,h);
    slider_start.setup( "end",0,0,1,x,y,w,h);

    button_name.setup( x+500, y, 200, 18, true );
	button_name.value = 255;
}

bool grafEditor::update(int mx, int my)
{
    if( button_name.update(mx,my) )
    {
        //cout << "editor edit!" << endl;
		bEditName = true;
       // return false;
    }else{
        button_name.bOn = true;
		bEditName = false;
    }

	bEditName =  !button_name.bOn;
	if(bEditName) return false;
	
    bool bChanged = false;

    if( slider_end.hitTest(mx,my) )
    {
        cout << "hit slider" << endl;
        bChanged = true;

        int startX    = (int)(x + slider_start.getPctValue()*w);
        int endX      = (int)(x + slider_end.getPctValue()*w);


        if( abs(mx-startX) < abs(mx-endX) )
        {
            //if( startX+(mx-x) < endX )
                slider_start.update(mx,my);
        }else{
            //if( endX+(mx-x) > startX )
                slider_end.update(mx,my);
        }


    }

    return bChanged;

}

void grafEditor::updateMax( int max )
{
    slider_start.setup( "start",0,0,max,x,y,w,h);
    slider_end.setup( "end",max-1,0,max,x,y,w,h);
}

void grafEditor::draw()
{
    ofSetColor(255,255,255,255);
    ofNoFill();
    ofRect(x,y,w,h);

    // start pt
    ofSetColor(255,0,0);
    ofCircle( x + slider_start.getPctValue()*w, y+h/2, 2);
    ofSetColor(255,255,255,255);
    ofCircle( x + slider_end.getPctValue()*w, y+h/2, 2);
	
	ofDrawBitmapString( "start", x + slider_start.getPctValue()*w, y-h*.25f);
	ofDrawBitmapString( "end", x + slider_end.getPctValue()*w, y-h*.25f);
	
	ofSetColor(255,255,255,255);
	button_name.draw();

    ofSetColor(200,200,200,255);
    ofDrawBitmapString("Graf Name: ", button_name.getX()-90, button_name.getY()+14);

	if(!button_name.bOn) ofSetColor(255,255,255,255);
	else				ofSetColor(0,0,0,255);
    if(grafName.size() > 0) ofDrawBitmapString( grafName, button_name.getX()+3, button_name.getY()+14);
	
	ofSetColor(200,200,200,255);
	ofDrawBitmapString( "Adjust start and end position of tag",slider_start.hitArea.x, slider_start.hitArea.y+34);
	ofDrawBitmapString( "To Save: click out of graf name text box + hit 's' ",button_name.getX()-90, slider_start.hitArea.y+34);


}

int grafEditor::getStart()
{
    return slider_start.getIntValue();
}


int grafEditor::getEnd()
{
    float sEnd = slider_end.getFloatValue();
    return (ceil)(sEnd);
}

void grafEditor::editName( char key )
{
    cout << (int) key << endl;
    if( grafName == "" ) grafName.clear();

    if(  grafName.size() > 0 && (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE)  ){

         //if( grafName.size()== 1 ) grafName.clear();
         grafName.erase(grafName.size()-1);
    }
    else if( (int)key > 32 && (int)key < 126 && grafName.size() < 35 ) grafName.push_back(key);
}

void grafEditor::setDefaultName( string defaultName )
{
	grafName.clear();
	grafName = defaultName + "_" + ofToString(nameCounter);
	nameCounter++;
}

string grafEditor::getName()
{

    if( grafName.size() == 0 ) return " ";
    if( nameCounter > 0 )  return grafName + "_" + ofToString(nameCounter);
    else return grafName;

}

void  grafEditor::checkNameForSaving(string directory)
{
    if( grafName == "" )
    {
        grafName.clear();
        grafName += ofToString(nameCounter);
        nameCounter++;
    }
	
	string dir = ofToDataPath(directory, true);
    if( PL.existsDirectory(dir,grafName) )
	{
        cout << "exists!" << endl;
        nameCounter++;
    }else
        nameCounter = 0;


    lastName = grafName;
}
