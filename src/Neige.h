//
//  Neige.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef Hey_Neige_h
#define Hey_Neige_h

#include "AppViz.h"
#include "ofApp.h"


class Neige : public AppViz{
    
public:
    Neige(ofApp *a):AppViz(a){
        params.setName("Neige");
        ofLog() << "create";
        CPARAM(useHPCP,false,false,true);
        CPARAM(threshCreate,0,0,1);
        CPARAM(ttlReduce,true,false,true);
        CPARAM(radius,0,0,600);
        CPARAM(randomNum,0,0,200);
        CPARAM(randomTime,0.,0,1.);
        CPARAM(randomRadius,0.01,.1,1);
                CPARAM(randomCenter,ofVec2f(0.5),ofVec2f(0),ofVec2f(1));
        CPARAM(distortX,1,0,2);
        CPARAM(gttl,2,0,10);
        CPARAM(ttlAlpha,true,false,true);
        CPARAM(soft,false,false,true);
        CPARAM(timeMask,1,0,10);

        parts.clear();
    }
    
    
    
    
    
    unsigned long lastTime = 0;
    
    
    
    
    ofParameter<bool> useHPCP;
    ofParameter<float> threshCreate;
    ofParameter<float> gttl,distortX;
    ofParameter<bool> ttlReduce;
    ofParameter<float> radius;
    ofParameter<bool> ttlAlpha;
    ofParameter<float> timeMask;
    ofParameter<float> randomTime;
    ofParameter<bool> soft;
    ofParameter<float > randomRadius;
    ofParameter<ofVec2f> randomCenter;
    unsigned long lastRandom=0;
    float curRandom = 0;;
    ofParameter<float> randomNum;
    int maxPart = 6000;
    vector<float> god;
    vector<float> timeMasks;
    
    
    
    void update()  {
        float delta = ofGetElapsedTimeMillis()-lastTime;
        updateTTL(delta);
        checkForNewToCreate(delta);
     
        
        lastTime = ofGetElapsedTimeMillis();
    }
    
    void draw() {
        for(int i = 0 ; i < parts.size() ;i++){
            parts[i].draw(ttlReduce,ttlAlpha,soft);
        }
    }
    
    void checkForNewToCreate(float delta){
        if(useHPCP?hasHPCP():hasMFCC()){
            god = useHPCP?getHPCP(true):getMFCC(true);
        }
        if(god.size()!=timeMasks.size()){
            timeMasks.resize(god.size());
            for(auto &t : timeMasks){
                t = 1000*timeMask;
            }
        }
        for(auto &t : timeMasks){
            t-=delta;
        }
        int idx = 0;
        for(auto & g : god){

            if(g>threshCreate && timeMasks[idx]<=0){
                ofVec3f pos = ofVec2f(ofRandom(0,1)*randomRadius,0);
                pos.rotate(0,0,ofRandom(0,360));
                pos.x=(pos.x +randomCenter->x)*app->widthOut;
                pos.y=( pos.y + randomCenter->y)*app->heightOut;
                float rad = radius*ofRandom(1);
                parts.push_back(particle(pos,rad,gttl.get()*1000.0));
                timeMasks[idx] = 1000*timeMask;
                ofLog() << "createG";
            }
            idx++;
        }
        if(randomNum>0){
            if(ofGetElapsedTimeMillis() - lastRandom> curRandom){
                ofLog() <<ofGetElapsedTimef();
                for(int i = 0 ; i < randomNum;i++){
                    ofVec3f pos = ofVec2f(ofRandom(0,1)*randomRadius,0);
                    pos.rotate(0,0,ofRandom(0,360));
                    
                    pos.x*= 2- ofClamp(distortX.get(), 1, 2);
                    pos.y*=ofClamp(distortX.get(), 0, 1);
                    pos.x=(pos.x +randomCenter->x)*app->widthOut;
                    pos.y=( pos.y + randomCenter->y)*app->heightOut;
                    float rad = radius*ofRandom(1);
                    parts.push_back(particle(pos,rad,gttl.get()*1000.0));
                }
                
                curRandom = randomTime*1000.0;
                lastRandom = ofGetElapsedTimeMillis();
            }
        }
    }
    
    void updateTTL(float delta){
        vector<particle> toKeep;
        for(int i = 0 ; i < parts.size() ;i++){
            parts[i].update(delta);
            if((parts[i].ttl>0 && i<maxPart)){
                toKeep.push_back(parts[i]);
            }

        }
        
        parts.clear();
        parts = toKeep;
        
    }
    
    
    class particle : public ofVec3f{
    public:
        particle(ofVec3f & v,float radius,float ttl):ofVec3f(v),ttl(ttl),originTtl(ttl),radius(radius){
            ofLog() << "createP";
            if(!dot.isAllocated()){
                dot.load("shaders/dot.png");
            }
        };
        static ofImage dot;
        float ttl;
        float originTtl;
        float radius;
        void draw(bool ttlReduce,bool ttlAlpha,bool soft){
            float cTTl = soft?(1-abs((ttl - originTtl/2)/originTtl)):(ttl/originTtl);
            float r = ttlReduce?cTTl*radius:radius;
            ofVec3f pos = *this;
            ofSetColor(255,255,255, ttlAlpha?cTTl*255:255);
            dot.draw(pos.x-r/2,pos.y-r/2,r,r);
//            ofDrawEllipse(pos,r,r);
//            ofLog() << "dr";
        }
        
        
        void update(float delta){
            ttl-=delta;
        }
    };
    vector<particle> parts;
    
};
ofImage Neige::particle::dot;
#endif
