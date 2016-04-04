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
#include "ofxOscParameterSync.h"


class ofApp;


class VizuHandler : public ofxMidiListener{
    
public:
    VizuHandler():curVizu(nullptr),nextVizu(nullptr),panel(nullptr){
        midiIn.listPorts();
        string portName = "";
        params = new ofParameterGroup();
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

        CPARAM(vizNumber,1,0,10);
        CPARAM(preset,0,0,5);
        CPARAM(transitTime,0,0,4);
        
        CPARAM(mainColor,ofColor(255),ofColor(0,0,0,0),ofColor(255));
        CPARAM(editMode,true,false,true);

        
        
        preset.addListener(this,&VizuHandler::presetChanged);
        vizNumber.addListener(this,&VizuHandler::vizChanged);
        
    };

    ~VizuHandler(){
        delete params;
    }
    
    ofxOscParameterSync  oscParam;
    string mainSavePath;
    void linkApp(ofApp * a){app =a;};
    ofApp * app;
    AppViz* curVizu;
    AppViz * nextVizu;
    unsigned long startTransition;

    float transitPct;
    bool isInTransit = false;
    int curNum = -1;
    ofxPanel * panel;
    ofxMidiIn midiIn;
    ofParameter<ofColor> mainColor ;
    ofParameter<bool> editMode;
    ofParameter<float> transitTime;
     ofParameter<int> vizNumber,preset;
    
    int setLeafChild(ofParameterGroup * pg,int root,int idx,int value);
    void draw(){
        ofEnableAlphaBlending();
        if(curVizu){
            ofColor curCol = mainColor;
            curCol.a =isInTransit?curCol.a*(1-transitPct):curCol.a;
            ofSetColor(curCol);
            curVizu->draw();
        }
        if(nextVizu){
            ofColor curCol = mainColor;
            curCol.a =isInTransit?curCol.a*(transitPct):curCol.a;
            ofSetColor(curCol);
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
                transitPct = 1;
                
                if(curVizu){
                oscParam.setup(*curVizu->params,7555,"localhost",7556);
                }
                
            }
        }
    }
    
    
    void setNext(int num,float time){
        
        mainSavePath = ofToDataPath("../../../preset/",true);
        mainSavePath+="/";
        ofLog() << "saving preset in " << mainSavePath;
        if(num == curNum)return;
        if(nextVizu)delete nextVizu;
        nextVizu = createForNum(num);
        isInTransit = true;
        transitTime = time;
        startTransition = ofGetElapsedTimeMillis();
        curNum = num;
        if(nextVizu){
            if(panel!=nullptr)delete panel;
            string pathSave = mainSavePath+ nextVizu->params->getName()+"_0"+".xml";
            panel = new ofxPanel(*nextVizu->params,pathSave);
           if(!editMode) panel->loadFromFile(pathSave);
            panel->setPosition(250, 10);
        }
    }
    
    void setPreset(int p){
        
        if(panel!=nullptr && curVizu!=nullptr){
            string presetName = mainSavePath+ curVizu->params->getName()+"_"+ofToString(p)+".xml";
            delete panel;
            panel = new ofxPanel(*curVizu->params,presetName);
            panel->setPosition(250, 10);
            if(!editMode)
                panel->loadFromFile(presetName);
        }
    }
    
    void newMidiMessage(ofxMidiMessage& msg)override;
    
    AppViz * createForNum(int num);
    
    bool changeNumViz;
    int numViz;
    ofParameterGroup * params;
    
   
    
    void vizChanged(int & num){
        setNext(num, transitTime);
    }
    void presetChanged(int & p){
        setPreset(p);
    }
    
    
//    bool parseOsc(ofxOscMessage &m){
//        return false;
////        vector<string> ad  =ofSplitString(m.getAddress(),"/");
////        if(curVizu!=nullptr && ad.size()>1){
////            ofParameterGroup pg = curVizu->params;
////            int depth = 0;
////            ofxOscArgType type = m.getArgType(0);
////            for(int i = 0 ; i < pg.size() ; i++){
////                ofParameter p = pg.get(i);
////                if(p.getName() == ad[depth]){
////                    if(p.isGroup)
////                }
//////                if(type = OFXOSC_TYPE_FLOAT){
//////                    bool hasChild = pg.getBool(<#const string &name#>)
//////                }
//////                else if(type = OFXOSC_TYPE_INT32){
//////                    
//////                }
////            }
////            
////        }
////        else{
////            return false;
////        }
//    }
    
    
};

#endif
