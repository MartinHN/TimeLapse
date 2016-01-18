//
//  GloveInstance.h
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//
// Class representing a glove instance




#ifndef __ImMedia__GloveInstance__
#define __ImMedia__GloveInstance__

#include <stdio.h>


#include "ofMain.h"

#include "ConfigGlove.h"

#include "ofxTweener.h"


    


class touchEventArgs : public ofEventArgs{
public:
    TouchButton touchId;
    TouchAction state;
};




class swipeEventArgs : public ofEventArgs{
public:
    TouchButton touchId;
    int axe;
    bool dir;

};



class GloveInstance {
public:

//    GloveInstance();
    GloveInstance(string _gloveID);
    ~GloveInstance();
    
    
    string gloveID;
    int cursorSize = 40;
    
    static vector<GloveInstance*> gloves;
    static ofEvent<ofVec3f > orientationEvent;
    static ofEvent<ofVec3f > relativeOrientationEvent;
    static ofEvent<touchEventArgs > touchEvent;
    static ofEvent< vector < float> > flexEvent;
    static ofEvent<ofVec2f > cursor2DEvent;
    static ofEvent<swipeEventArgs > swipeEvent;
    static ofEvent<string > calibEvent;

    
    static GloveInstance * getGlove(string gloveID);
    static void deleteGlove(string gloveID);
    static void deleteAllGloves();
    static  bool gloveExists(string gloveID);
    
    
    void update(ofEventArgs & a);
    void draw(ofEventArgs & a);
    

    
    //GloveState
    
    ofVec3f orientation;
    ofVec3f relativeOrientation;
    vector<bool>  touchs;
    vector<float>  flex;
    ofVec2f cursor2D;
    ofVec2f cursor2DRaw;
    bool isActive;
    bool isFreezed;
    
    ofVec3f orientationVel;
    ofVec3f relativeOrientationVel;
    ofVec2f cursor2DVel;
    
    
    void setOrientation(ofVec3f o);
    void setRelativeOrientation(ofVec3f o);
    void setTouch(TouchButton t,TouchAction a);
    void setFlex(int n,float f);
    void setCursor2D(ofVec2f c);

//    ofImage cursorImg;
//    ofImage cursorImg_Click;
//    ofImage cursorImg_Action;
//    ofImage cursorImg_ArcCircle_Discret;
//    ofImage cursorImg_ArcCircle_Continu;
//    ofImage cursorImg_Center_Discret;
//    ofImage cursorImg_Center_Continu;
    bool actionCursor=false;
    bool isCalib = false;
    float lockedAxe=0;
    int lastLockedAxe;

//    void setCursor2DFromGoss(int WallIdx,ofVec2f v);
    

    
private:
        float lastTouchDown;
    ofVec2f lastActionCursor;
    
    float cursorAlpha = 255;
    bool isLocking = false;
    // display cursors Function

    bool swipeDir;

    void drawRelativeInfos();
    float clickAlpha;
    
    
    vector<float> cursorMedianX;
    vector<float> cursorMedianY;
    
    ofRectangle cursorLim;
    
};





#endif /* defined(__ImMedia__GloveInstance__) */
