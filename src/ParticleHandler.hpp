 //
//  ParticleHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#ifndef ParticleHandler_hpp
#define ParticleHandler_hpp


//#define SPLIT_THREAD

#include <stdio.h>

#include "ofMain.h"
#include "ForceHandler.hpp"
#include "PhysicsHandler.hpp"
#include "ConfigParticles.h"
#include "ofxNearestNeighbour.h"




class MultiParticleHandler;

class ParticleHandler{
public:
    ParticleHandler(MultiParticleHandler * _owner):owner(_owner){
        params.setName("partGroup");
        setFPS(30);
        nn = make_shared<MyNN>();
        physics = new PhysicsHandler(this);
        forceHandler =  new ForceHandler(this);

        
        
        init();
        CPARAM(lineStyle,0,0,10);
//        lineStyle.addListener(this,&ParticleHandler::changedLineStyle);
        CPARAM(originType,0,0,10);
        CPARAM(kNN,6,-1,20);
        kNN.addListener(this ,&ParticleHandler::changedLineStyle);
        params.add(forceHandler->forcesParams);
        params.add(physics->params);
        originType.addListener(this ,&ParticleHandler::changeOrigin);

    };
    
    ~ParticleHandler(){
        physics->waitForThread(true);
#ifdef SPLIT_THREAD
        forceHandler->waitForThread(true);
#endif
        delete forceHandler;
        delete physics;
    }
    
    
    int side;
    void init();
    void changeOrigin(int & type);
    
    void start(){
        resetToInit();
#ifdef SPLIT_THREAD
        forceHandler->startThread();
#endif
        physics->startThread();
    }
    void changedLineStyle(int & s){
        //avoid doublons from mouse release, not so pretty..
        
        if(kNN==lastkNN)
            return;
        initIndexes();
        forceHandler->changeNumParticles(numParticles);
        lastkNN = kNN;
        
    }
    
    ofParameterGroup params;
    ofParameter<int> lineStyle;
    ofParameter<int> originType;
    ofParameter<int > kNN;
    int lastkNN;
    int lastOriginType;
    
    void stopForces()
    {if(forceHandler!=nullptr){
#ifdef SPLIT_THREAD
        forceHandler->waitForThread(true);
#else
        physics->waitForThread(true,1000);
#endif
    }
    
    };
    
    void startForces(){
#ifdef SPLIT_THREAD
        forceHandler->startThread();
#else
        physics->startThread();
#endif
    };
    
    
    void initGrid(int num,bool flat);
    void initIndexes();
    
    
    
    double getWidthSpace();
    
    MyMatrixType position,positionInit;
#if NEED_TO_CAST_VERT
    Matrix<float,ROWTYPE,MyMatrixType::ColsAtCompileTime,RowMajor|AutoAlign>floatPos;
#endif
    MyMatrixType velocity;
    MyMatrixType acceleration;
    
    
    int numParticles;
    uint64_t deltaT;
    mutex mutex;
    
    ofVbo vbo;
    
    vector<ofVec3f> sizes;
    typedef std::pair<ofIndexType,ofIndexType>  IndexType;
    vector<IndexType> indexes;
    vector<unsigned int> lineIdx;
    vector<float> lineDists;
    void setNumParticles(int num);
    void setFPS(int fps){
        deltaT = 1000.0/fps;
    }
    void loadModel(ofFile f);
    void update();
    void draw();
    void drawLines();
    void resetToInit();
    ForceHandler *   forceHandler;
    PhysicsHandler *  physics;
    
    MultiParticleHandler * owner;
    
    
    
    typedef itg::NearestNeighbour<Array<float,COLNUM,1> >  MyNN;
    shared_ptr<MyNN>  nn;
    

};

#endif /* ParticleHandler_hpp */
