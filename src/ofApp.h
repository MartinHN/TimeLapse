#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"
#include "AppViz.h"
#include "ofxMidi.h"
#include "ofxGUI.h"
#ifdef LEAPMOTION
#include "ofxLeapMotion.h"
#endif
#include "VizuHandler.h"


//#include "GloveOSC.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    void exit();
    
    void parseOsc();


    
    vector<float> HPCP;
    vector <float> MFCC;
    vector<float> enveloppes;
    int cluster;
    bool clusterUpdated;
    map<string,ofVec3f>    attractors;
    #ifdef LEAPMOTION
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
#endif
    void removeLeapAttractors();
    void updateLeap();
    ofxOscReceiver osc;
    
    ofxSyphonServer syphon;
    bool hasOSC;
    
    ofFbo outTexture;
    
     int widthOut = 1920;
     int heightOut = 1080;
    


    
    ofParameter<int> vizNumber,preset,alphaFade;
    
//    ofParameter<ofColor> mainColor;
    VizuHandler viz;

    
    ofxPanel * panel;
        ofParameterGroup *  params;
    
//    GloveOSC  glove;

};
