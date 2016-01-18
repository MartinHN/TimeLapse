//
//  GloveOSC.cpp
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//

#include "GloveOSC.h"







ofxOscReceiver GloveOSC::reciever;
ofxOscSender GloveOSC::toServer;




GloveOSC::GloveOSC(){
    

}


GloveOSC::~GloveOSC(){
    
    for(  vector<GloveInstance*>::iterator it =GloveInstance::gloves.begin() ; it!= GloveInstance::gloves.end();++it){
        delete *it;
    }
}

void GloveOSC::init(){
    reciever.setup(LOCALPORT);
    toServer.setup(SERVERIP, SERVERPORT);
    cout << "server at :" << SERVERIP << endl;
    lastACK = 0;
    ofAddListener(ofEvents().update,this,&GloveOSC::update);
//    ofAddListener(drawSyphonEvent,this,&GloveOSC::draw);
    isConnectedToServer = false;
    isConnectedToServer.addListener(this, &GloveOSC::setConnected);
}
void GloveOSC::update(ofEventArgs & a){
    
    registerOSC();
    parseMessage();
    
    
}


void GloveOSC::draw(ofEventArgs & a){
    ofPushStyle();
    ofSetColor(!(GloveInstance::gloves.size()>1)?255:0,(GloveInstance::gloves.size()>1)?255:0,0);
    ofNoFill();
    ofSetLineWidth(5);
    ofDrawCircle(20, 20, 20);
    ofPopStyle();
}


void GloveOSC::registerOSC(){
    if( (ofGetElapsedTimef()-lastACK)>2){
        ofxOscMessage regMsg;
        regMsg.setAddress("/register");
        regMsg.addStringArg(APPNAME);
        regMsg.addStringArg(LOCALIP);
        regMsg.addIntArg(LOCALPORT);
        
        toServer.sendMessage(regMsg);

        
        lastACK = ofGetElapsedTimef();
        
    }
    
    if(ofGetElapsedTimef()-lastPingTime>4){
        isConnectedToServer=false;
    }
    
}



void GloveOSC::parseMessage(){
    
    
    while(reciever.hasWaitingMessages()){
        
        ofxOscMessage m;
        reciever.getNextMessage(&m);
        
        string addr = m.getAddress();
        GloveInstance *curGlove = NULL;
        //        cout << m.getArgAsString(0) << endl;
        

        
        //from Server
         if((addr.find_first_of("/glove"))==0){
            addr = addr.substr(6,addr.length()-6);
            
            // Glove Registration ACK
            if(addr == "/registered"){
                isConnectedToServer= true;
                
            }
            else if(addr == "/unregistered"){
                isConnectedToServer = false;
                
                
            }
            else if(addr == "/connected"){
                
                if(!GloveInstance::gloveExists(m.getArgAsString(0))){
                    GloveInstance::gloves.push_back(new GloveInstance(m.getArgAsString(0)));
                }
                
            }
            else if(addr == "/disconnected"){
                GloveInstance::deleteGlove(m.getArgAsString(0));
            }
            
            else if(addr == "/active"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    curGlove->isActive = m.getArgAsInt32(1)==1;
                }
            }
            
            else if(addr == "/orientation"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    ofVec3f orientation(m.getArgAsFloat(1),m.getArgAsFloat(2),m.getArgAsFloat(3));
                    curGlove->setOrientation ( orientation);
                }
            }
            else if(addr == "/relative"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    curGlove->setRelativeOrientation (ofVec3f(m.getArgAsFloat(1),m.getArgAsFloat(2),m.getArgAsFloat(3)));
                }
            }
            else if(addr == "/touch"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    curGlove->setTouch((TouchButton)(m.getArgAsInt32(1)+1),(TouchAction)m.getArgAsInt32(2));
                }
            }
            else if(addr == "/flex"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    curGlove->setFlex(m.getArgAsInt32(1), m.getArgAsFloat(2));
                }
            }
            else if(addr == "/cursor2D"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    curGlove->setCursor2D ( ofVec2f(m.getArgAsFloat(1),m.getArgAsFloat(2)));
                }
            }
            else if(addr == "/cursorCalibState"){
                if((curGlove = GloveInstance::getGlove(m.getArgAsString(0)))){
                    string command =m.getArgAsString(1);
                    ofNotifyEvent(GloveInstance::calibEvent, command, curGlove);
                }
            }
            
            
        }
    }
    
}


    
    void GloveOSC::unregisterOSC(){
        ofxOscMessage m;
        m.setAddress("/unregister");
        m.addStringArg(APPNAME);
        toServer.sendMessage(m);
    }
    
    
    void GloveOSC::setConnected(bool &b){
        if(b){
            lastPingTime = ofGetElapsedTimef();
        }
        else {
            GloveInstance::deleteAllGloves();
        }
    }
    
    
    
    
