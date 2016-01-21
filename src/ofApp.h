#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxSyphon.h"
#include "AppViz.h"
#include "ofxMidi.h"
#include "ofxGUI.h"
#include "ofxLeapMotion.h"

#include "VizuHandler.h"


#include "GloveOSC.h"

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

    void vizChanged(int & num);
    
    vector<float> HPCP;
    vector <float> MFCC;
    vector<float> enveloppes;
    int cluster;
    
    map<string,ofVec3f>    attractors;
    ofxLeapMotion leap;
    vector <ofxLeapMotionSimpleHand> simpleHands;
    void removeLeapAttractors();
    void updateLeap();
    ofxOscReceiver osc;
    
    ofxSyphonServer syphon;
    bool hasOSC;
    
    ofFbo outTexture;
    
     int widthOut = 1280;
     int heightOut = 800;
    

    ofParameter<float> transitionTime;
    ofParameter<int> vizNumber,preset;
    ofParameter<bool> editMode;
    VizuHandler viz;
    void presetChanged(int & p);
    void changeVizu(int num);
    
    ofxPanel * panel;
        ofParameterGroup params;
    
    GloveOSC  glove;
		
};
