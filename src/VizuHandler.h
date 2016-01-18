//
//  VizuHandler.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef Hey_VizuHandler_h
#define Hey_VizuHandler_h

#include "ofMain.h"
#include "AppViz.h"
#include "ofxMidi.h"

class ofApp;


class VizuHandler : public ofxMidiListener{
    
public:
    VizuHandler():curVizu(nullptr),nextVizu(nullptr),panel(nullptr){
        midiIn.listPorts();
        string portName = "";
        for(auto &a:midiIn.getPortList()){
            if(ofSplitString(a, "nano").size()>1){
                portName = a;
                ofLog() << "connecting midi to " << a;
                break;
            }
        }
        
        if(portName!=""){
            midiIn.openPort(portName);
        }
        else{
            midiIn.openPort();
        }
        midiIn.addListener(this);
        
        
            alpha.set("alpha",1.0,0.,1.);
    };
    
    
    void linkApp(ofApp * a){app =a;};
    ofApp * app;
    AppViz* curVizu;
    AppViz * nextVizu;
    unsigned long startTransition;
    float transitTime;
    float transitPct;
    bool isInTransit = false;
    int curNum = -1;
    ofxPanel * panel;
    ofxMidiIn midiIn;
    ofParameter<float> alpha = 1.0;
    
    
    void draw(){
        ofEnableAlphaBlending();
        if(curVizu){
            ofSetColor(255,255,255,isInTransit?alpha*255*(1-transitPct):alpha*255);
            curVizu->draw();
        }
        if(nextVizu){
            ofSetColor(255,255,255,isInTransit?alpha*255*(transitPct):alpha*255);
            nextVizu->draw();
        }


    }
    
    void update();
    
    
    void updateTransition(){
        if(isInTransit){
            transitPct = (ofGetElapsedTimeMillis() - startTransition)*1.0/(transitTime*1000.0);
            if(transitPct>1){
                AppViz* last  = curVizu;
                curVizu = nextVizu;
                delete last;
                nextVizu = nullptr;
                isInTransit = false;

            }
        }
    }
    

    void setNext(int num,float time){
        

        if(num == curNum)return;
        if(nextVizu)delete nextVizu;
        nextVizu = createForNum(num);
        isInTransit = true;
        transitTime = time;
        startTransition = ofGetElapsedTimeMillis();
        curNum = num;
        if(nextVizu){
        if(panel!=nullptr)delete panel;
        string pathSave = ofToDataPath(nextVizu->params.getName()+".xml");
        panel = new ofxPanel(nextVizu->params,pathSave);
        panel->loadFromFile(pathSave);
        panel->setPosition(250, 10);
        }
    }
    
    
    
    void newMidiMessage(ofxMidiMessage& msg)override;
    
    AppViz * createForNum(int num);
    
    bool changeNumViz;
    int numViz;
    
    
    

    
    
    
};

#endif
