//
//  GlovePointer.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef Hey_GlovePointer_h
#define Hey_GlovePointer_h
#include "AppViz.h"
#include "ofApp.h"
#include "GloveInteract.h"


class GlovePointer : public AppViz,public GloveInteract{
    
public:
    GlovePointer(ofApp *a):AppViz(a){
        
        params.setName("GlovePointer");
        
//        ofAddListener(ofEvents().mouseDragged,this,&GlovePointer::mouseDragged);
//        ofAddListener(ofEvents().mousePressed,this , &GlovePointer::mousePressed);
//        ofAddListener(ofEvents().mouseReleased,this , &GlovePointer::mouseReleased);
//        ofAddListener(ofEvents().keyReleased,this , &GlovePointer::keyReleased);
        CPARAM(numParticules,1,1,1000);
        CPARAM(radius,30,1,300);
        CPARAM(reinit,false,false,true);
        CPARAM(invert,true,false,true);
        CPARAM(gloveDbg,true,false,true);
        CPARAM(isAttr,true,false,true);
        
        CPARAM(friction,1,0,1);
        
        CPARAM(origin,true,false,true);
        CPARAM(originK,.01,0,.01);
        
        CPARAM(spring,true,false,true);
        CPARAM(springK,.01,0,.01);
        CPARAM(springRadius,0,0,.5);
        
        numParticules.addListener(this,&GlovePointer::changeNum);
        reinit.addListener(this,&GlovePointer::applyOrigin);
        
        
        numParticules = 25;
        bool dumb =true;
        generateGrid();
        applyOrigin(dumb);
        top = ofVec2f(-8.7,27);
        bottom .set(151,-31);
    }
    ~GlovePointer(){

    }
    
    
    class particle{
    public:
        
        particle():pos(0),vel(0),mass(20){}
        ofVec3f pos;
        ofVec3f vel;
        float mass ;
        
        
    };
    
    
    vector<particle> part;
    vector<ofVec3f> origins;
    ofParameter<bool> gloveDbg;
    
    ofParameter<int> numParticules;
    ofParameter<float> radius;
    ofParameter <bool > reinit;
    ofParameter<bool> isAttr;
    
    ofParameter<bool> spring;
    ofParameter<float> springK;
    ofParameter<float> springRadius;
    
    
    ofParameter<bool> invert;
    
    
    ofParameter<bool> origin;
    ofParameter<float> originK;
    
    
    ofParameter<float> friction;
    
    
    ofVec3f attrPos;

    
    unsigned long lastTime;
    
    
    void changeNum(int & num){
        part.resize(num);
        origins.resize(num);
        generateGrid();
    }
    
    
    void generateGrid(){
        int snum = std::sqrt((int)numParticules);
        for(int i = 0 ; i < snum ; i++){
            for(int j = 0 ; j < snum ; j++){
                origins[i*snum + j] = ofVec3f(i*1.0/(snum-1),j*1.0/(snum-1),0);
            }
        }
    }
    
    
    void applyOrigin(bool & b){
        int idx = 0 ;
        for(auto & p:part){
            p.pos = origins[idx];
            p.vel.set(0,0,0);
            idx++;
        }
        
        
    }
    void mousePressed(int x, int y,int button) override{
        if(x > ofGetWidth()/2)        isAttr =true;
    }
    
    void mouseReleased(int x, int y,int button) override{
        isAttr = false;
    }
    void mouseDragged(int x, int y,int button)override{
        if(x > ofGetWidth()/2){isAttr = true;}
        attrPos =( ofVec2f(x,y) - ofVec2f(ofGetWidth()/2,0))/ofVec2f(ofGetWidth()/2,ofGetWidth()*getHeight()/getWidth()/2) ;
        ofLog() << attrPos;
    }
    
    void touchGlove( TouchButton num, TouchAction s){
        if(s <=1){
//        if(num == 12){
//            isAttr = s ;
//        }
         if(num == 12 && s ==0){
            isAttr = !isAttr;
        }
        }
    
    };
    void update() override{
    
    if(curGlove){
        attrPos = mapOrientation(curGlove->orientation);
    }
    float delta = ofGetElapsedTimeMillis() - lastTime;
    
    //AttrForces
    if(isAttr){
        if(spring)
            updateSpring();
            }
    
    
    if(origin)
        updateOrigin();
        
        // Apply
        applyVel(delta);
        
        lastTime = ofGetElapsedTimeMillis();
        }


void draw() override{



    ofSetColor(invert?0:255);
    ofDrawRectangle(0, 0, getWidth(), getHeight());
    
    
    
    ofSetColor(!invert?0:255);
    for(auto &p:part){
        ofDrawEllipse(p.pos*ofVec2f(getWidth(),getHeight()),radius,radius);
    }




if(gloveDbg){

    if(curGlove){
            ofSetColor(255,0,0);
//        ofVec3f Pos = curGlove->cursor2D*ofVec2f(getWidth(),getHeight());
        
        ofDrawEllipse(attrPos*ofVec2f(getWidth(),getHeight()),30,30);
    }
    
}

}

ofVec2f top,bottom;
ofVec3f mapOrientation(ofVec3f v){
    float cv = v.x+180;//(v.x-top.x);
    cv= (((cv>180)?(360 - cv):cv)/180.0);
    cv = 1-cv;
    ofLog() << cv;
    return ofVec3f(cv,///(bottom.x - top.x),
                    (v.y - top.y)/(bottom.y-top.y+ 1),
                   0);
    
}

void keyReleased(ofKeyEventArgs & k){
    if(curGlove){
    switch(k.key){
            case 't':
            top = curGlove->orientation;
            break;
            
            case 'b':
            bottom = curGlove->orientation;
            break;
            
            
            
    }
    }
    
}

void updateSpring(){
    for(auto & p:part){
        ofVec3f n = attrPos -p.pos  ;
        p.vel+= n.normalized() * (n.length() - springRadius)*springK;
    }
}

void updateOrigin(){
    int idx=0;
    for(auto & p:part){
        ofVec3f n = origins[idx] -p.pos  ;
        p.vel+= n*originK;
        idx++;
    }
}

void applyVel(float delta){
    for(auto & p:part){
        p.pos+=(p.vel*=friction)*delta;
    }
}


};

#endif
