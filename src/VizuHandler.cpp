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