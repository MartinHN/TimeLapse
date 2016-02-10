//
//  MultiParticleHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 14/01/2016.
//
//

#ifndef MultiParticleHandler_hpp
#define MultiParticleHandler_hpp

#include "ParticleHandler.hpp"
#include "AppViz.h"

#define APPLY_BEGIN \
{ int i =0; \
for(auto &f:particlesList){ \
    if(idx==-1 || i==idx){

#define APPLY_END \
} i++; \
}; \
}


#define APPLY_ON_EXISTING(X) X(int idx = -1) APPLY_BEGIN \
f->X(); \
APPLY_END

#define APPLY_ON_EXISTING1(X,T) X(T Y,int idx = -1) APPLY_BEGIN \
f->X(Y); \
APPLY_END


#define APPLY_ON_EXISTING_FORCE(X) X(int idx = -1) \
APPLY_BEGIN \
f->forceHandler->X(); \
APPLY_END


#define APPLY_ON_EXISTING_FORCE1(X,T) X( T Y,int idx = -1) APPLY_BEGIN \
f->forceHandler->X(Y); \
APPLY_END

#define APPLY_ON_EXISTING_FORCE2(X,T,T2) X( T Y,T2 Z,int idx = -1) APPLY_BEGIN \
f->forceHandler->X(Y,Z); \
APPLY_END

#define EXPOSE_ARG(X,T) set##X(T Y,int idx = -1) APPLY_BEGIN \
f->X = Y; \
APPLY_END

class MultiParticleHandler :public AppViz{
public:
    MultiParticleHandler(ofApp* a):AppViz(a){
        params.setName("Particles");
        CPARAM(pointSize,8,0,50);
        CPARAM(lineWidth,8,0,50);
        CPARAM(drawAttr , false,false,true);
        CPARAM(zoom,1,0.1,10);
        CPARAM(distortFactor,1,0.01,10);
        CPARAM(defaultNumPart,10000,27,200000);
        
        
        
        particlesList.resize(NUMTHREAD_PART);
        initGL();
        for(auto &f:particlesList){
            f = new ParticleHandler(this);
            params.add(f->params);
        }
        start();
        
        
        for(auto &f:particlesList){
            f->originType = 0;
        }
    }
    ~MultiParticleHandler(){

        for(auto &f:particlesList){
            delete f ;
        }
    }
    
    void initGL();
    void renderOnGPU(bool t);
    bool isRenderingOnGPU;
        ofEasyCam cam;
    float widthSpace ;
    ofShader shader;
    ofTexture pointTexture;
    ofVec3f screenToWorld(ofVec3f v);
    ofVec3f normalizedToWorld(ofVec3f v);

        void draw() override;
    vector<ParticleHandler*> particlesList;
    
    
    ofParameter<float> pointSize,lineWidth,zoom,distortFactor;
    ofParameter<int > defaultNumPart;
    ofParameter<bool> drawAttr;
    void update() override;
    

    void APPLY_ON_EXISTING(start);
    void APPLY_ON_EXISTING(update);
//    void APPLY_ON_EXISTING(draw);

    void APPLY_ON_EXISTING(stopForces);
    void APPLY_ON_EXISTING(startForces);
    void APPLY_ON_EXISTING(resetToInit);
    void APPLY_ON_EXISTING1(loadModel,ofFile);

    
    typedef map<string,ofVec3f> attractorMap;
    void APPLY_ON_EXISTING_FORCE1(activateForce,string);
    void APPLY_ON_EXISTING_FORCE2(activateForce,string,bool);
    void APPLY_ON_EXISTING_FORCE2(setAttractor,string,ofVec3f);
    void APPLY_ON_EXISTING_FORCE1(removeAttractor,string);
    void APPLY_ON_EXISTING_FORCE(clearAttractors);

    void APPLY_ON_EXISTING_FORCE1(resetAttractors, attractorMap);
    
    
    
    

    vector<ofFile> models;
    int idxModel;
    //--------------------------------------------------------------
    void keyReleased(int key) override{
        switch(key){
            case 'x':
                stopForces();
                resetToInit();
                startForces();
                break;
            case 'g':
                renderOnGPU(!isRenderingOnGPU);
                
                break;
            case 'o':
                activateForce("origin",false);
                
                break;
            case 's':
//                stopForces();
//                loadModel(models[(idxModel++)%models.size()].getFileName());
//                startForces();
                break;
        }
    }
};

#endif /* MultiParticleHandler_hpp */
