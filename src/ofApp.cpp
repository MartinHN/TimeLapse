#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    viz.linkApp(this);
    
    ofBackground(0);
    osc.setup(6001);
#ifndef DEBUG
    ofLog() << ofToDataPath("",true);
    ofSetDataPathRoot("/");
    ofLog() << ofToDataPath("",true);
#endif
    
    syphon.setName("HeyOF");
    
    
    ofSetFrameRate(50);
    
    outTexture.allocate(widthOut, heightOut);
    
    
    leap.open();
    
    CPARAM(vizNumber,1,0,10);
    CPARAM(transitionTime,2,0,6);
    CPARAM(preset,0,0,5);
    CPARAM(editMode,true,false,true);
    preset.addListener(this,&ofApp::presetChanged);
    vizNumber.addListener(this,&ofApp::vizChanged);
    
    params.add(viz.alpha);
    
    
    params.setName("params");
    panel = new ofxPanel(params);
    
    vizNumber = 0;
    
    glove.init();
    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    updateLeap();
    parseOsc();
    viz.update();
}


void ofApp::vizChanged(int & num){
    viz.setNext(num, transitionTime);
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    outTexture.begin();
    ofSetColor(0);
    ofDrawRectangle(0, 0, widthOut, heightOut);
    ofSetColor(255);
    viz.draw();
    outTexture.end();
    
    syphon.publishTexture(&outTexture.getTexture());
    
    if(hasOSC)ofDrawEllipse(30,30,30,30);
    ofDisableAlphaBlending();
    ofSetColor(255,255,255,255);
    outTexture.draw(0, 0,ofGetWidth(),ofGetHeight());
    
    if(panel)panel->draw();
    
    
    if(viz.panel)viz.panel->draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}
void ofApp::removeLeapAttractors(){
    auto attrTmp = attractors;
    for(auto & l:attractors){
        vector <string > add = ofSplitString(l.first,"/");
        
        if(add.size()>1 && add[1] == "leap"){
            ofLog() << add[0];
            attrTmp.erase(l.first);
        }
    }
    attractors = attrTmp;
}
void ofApp::updateLeap(){
    simpleHands = leap.getSimpleHands();
    if( leap.isFrameNew()){
        leap.setMappingX(-80, 80, 0,1);
        leap.setMappingY(0,400, 0,1);
        leap.setMappingZ(-200,200, 0.1, 1);
        removeLeapAttractors();
        if(simpleHands.size() ){

            
            
            for(int i = 0; i < simpleHands.size(); i++){
                
                for(int j = 0; j < simpleHands[i].fingers.size(); j++){
//                    int id = simpleHands[i].fingers[j].id;
                    string id = "/leap/"+ofToString(i)+"/"+ofToString(j);
                    //store fingers seen this frame for drawing
                    attractors[id]=simpleHands[i].fingers[j].pos;

                    
                }
            }
        }

    }
}
void ofApp::parseOsc(){
    ofxOscMessage m;
    hasOSC = false;
    while(osc.getNextMessage(m)){
        hasOSC = true;
        
        vector<string> ad  =ofSplitString(m.getAddress(),"/");
        //        ofLog() << m.getAddress();
        if(ad[1] == "attractor"){
            int attractor = m.getArgAsInt32(0);
            string attractName = ofToString(attractor);
            if(m.getNumArgs() == 2){
                attractors.erase(attractName);
            }
            else{
                
                attractors[attractName] = ofVec3f(m.getArgAsFloat(1),m.getArgAsFloat(2),0);
                
            }
            
        }
        if(ad.size()<2)continue;
        if(ad[1] == "enveloppes"){
            if(m.getNumArgs()!=enveloppes.size()){enveloppes.resize(m.getNumArgs());}
            for(int i = 0 ; i < m.getNumArgs();i++){enveloppes[i] = m.getArgAsFloat(i);}
        }
        if(ad.size()<3)continue;
        int num = ofToInt(ad[1]);
        if(ad[2] == "HPCP"){
            if(m.getNumArgs()!=HPCP.size()){HPCP.resize(m.getNumArgs());}
            for(int i = 0 ; i < m.getNumArgs();i++){HPCP[i] = m.getArgAsFloat(i);}
        }
        
        else if(ad[2] == "MFCC"){
            if(m.getNumArgs()!=MFCC.size()){MFCC.resize(m.getNumArgs());}
            for(int i = 0 ; i < m.getNumArgs();i++){MFCC[i] = (1+m.getArgAsFloat(i))/2;}
        }
        else if(ad[2] == "cluster"){
            cluster = m.getArgAsInt(0);
        }
        
    }
}

void ofApp::presetChanged(int & p){
    viz.setPreset(p,!editMode);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    if(button ==2){
        
        //        ofVec3f a = ofVec3f(x,y);//screenToWorld(ofVec2f(x,y));
        attractors["mouse"] = ofVec3f(x*1.0/ofGetWidth(),1.0-y*1.0/ofGetHeight());
        //                                  ofLog() <<         a;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    attractors.erase("mouse");
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
void ofApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
