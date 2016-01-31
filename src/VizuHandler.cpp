//
//  VizuHandler.cpp
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#include "VizuHandler.h"

#include "VizWarpper.h"

void VizuHandler::update(){
    updateTransition();
    if(curVizu!=nullptr){curVizu->internUpdate();curVizu->update();}
    if(nextVizu!=nullptr){nextVizu->internUpdate();nextVizu->update();}
    if(changeNumViz){
        app->vizNumber = numViz;
        changeNumViz = false;
    }
}

AppViz * VizuHandler::createForNum(int num){
    switch (num){
        case 0:
            return new SignalDrawer(app);
        case 1:
            return new Bars(app);
        case 2:
            return new CircleDrawer(app);
        case 3:
            return new Neige(app);
        case 4:
            return new GlovePointer(app);
        case 5:
            return new MultiParticleHandler(app);
        case 6:
            return new TimeLapser(app);
        case 7 :
            return new TreeLapser(app);
            
        default:
            return nullptr;
            break;
            
            
    }
}


void VizuHandler::newMidiMessage(ofxMidiMessage& msg){
    
    
    if(curVizu!= nullptr && msg.status == MIDI_CONTROL_CHANGE){
        // global Links
        
        if(msg.control == 16){
//            curVizu->smooth.set( ofMap(msg.value,0,127,0.0,1.0));
//            curVizu->smoothDown.set( ofMap(msg.value,0,127,0.0,1.0));
            return;
        }
        if(msg.control == 17){
//            alpha.set(ofMap(msg.value,0,127,0.0,1.0));
        }
        if( msg.value == 0 &&( msg.control >=64 && msg.control <=70)){
             numViz =  msg.control - 64;
            changeNumViz = true;

        }
        
        
        //dynamic ones
        
        int num = msg.control;

        if(num >=0 && num < curVizu->params.size()){
            
            ofParameter<int> * pi =  dynamic_cast<ofParameter<int>* > (&curVizu->params.get(num));
            if(pi){
                pi->set(ofMap(msg.value,0,127,pi->getMin(),pi->getMax()));
                return;
            }
            ofParameter<float> * pf =  dynamic_cast<ofParameter<float>* > (&curVizu->params.get(num));
            if(pf){
                pf->set(ofMap(msg.value,0,127,pf->getMin(),pf->getMax()));
                return;
            }
            ofParameter<bool> * pb =  dynamic_cast<ofParameter<bool>* > (&curVizu->params.get(num));
            if(pb){
                pb->set(msg.value>64);
                return;
            }
            
        }

        
        
    }
}

int VizuHandler::setLeafChild(ofParameterGroup * pg,int rootIdx,int destIdx,int value){
    int count = 0;
    for(int i = 0 ; i < pg->size();i++){
        if(rootIdx + count ==destIdx){
            ofParameter<int> * pi =  dynamic_cast<ofParameter<int>* > (&curVizu->params.get(count));
            if(pi){
                pi->set(ofMap(value,0,127,pi->getMin(),pi->getMax()));
                return -1;
            }
            ofParameter<float> * pf =  dynamic_cast<ofParameter<float>* > (&curVizu->params.get(count));
            if(pf){
                pf->set(ofMap(value,0,127,pf->getMin(),pf->getMax()));
                return -1;
            }
            ofParameter<bool> * pb =  dynamic_cast<ofParameter<bool>* > (&curVizu->params.get(count));
            if(pb){
                pb->set(value>64);
                return -1;
            }
            ofParameter<ofVec2f> * pv =  dynamic_cast<ofParameter<ofVec2f>* > (&curVizu->params.get(count));
            if(pv){
                *pv = ofVec2f(ofMap(value,0,127,pv->getMin().x,pv->getMax().x),pv->get().y);
                return -1;
            }
            ofParameter<ofVec3f> * pv3 =  dynamic_cast<ofParameter<ofVec3f>* > (&curVizu->params.get(count));
            if(pv3){
                *pv3 = ofVec3f(ofMap(value,0,127,pv3->getMin().x,pv3->getMax().x),pv3->get().y,pv3->get().z);
                return -1;
            }
        }
        
        if(destIdx==rootIdx + count +1 ){
            ofParameter<ofVec2f> * pv =  dynamic_cast<ofParameter<ofVec2f>* > (&curVizu->params.get(count));
            if(pv){
                *pv = ofVec2f(pv->get().x,ofMap(value,0,127,pv->getMin().y,pv->getMax().y));
                return -1;
            }

        }
        
        
        
        if(dynamic_cast<ofParameter<int>* > (&curVizu->params.get(i)) ||
           dynamic_cast<ofParameter<float>* > (&curVizu->params.get(i)) ||
           dynamic_cast<ofParameter<bool>* > (&curVizu->params.get(i)) ){
            count++;
            
        }
        else if(dynamic_cast<ofParameter<ofVec2f>* > (&curVizu->params.get(i))){
            count+=2;
        }
        else if(dynamic_cast<ofParameter<ofVec3f>* > (&curVizu->params.get(i))){
            count+=3;
        }
        else if(dynamic_cast<ofParameterGroup* > (&curVizu->params.get(i))){
            int res =setLeafChild(dynamic_cast<ofParameterGroup* > (&curVizu->params.get(i)),count,destIdx,value);
            if(res == -1 ){
                return -1;
            }
            count+=res;
        }

    }
    
}