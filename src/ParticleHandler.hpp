//
//  ParticleHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#ifndef ParticleHandler_hpp
#define ParticleHandler_hpp

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
        setFPS(50);
        nn = new MyNN();
        forceHandler =  new ForceHandler(this);
        physics = new PhysicsHandler(this);
        
        init();
  
    };
    
    ~ParticleHandler(){
        physics->waitForThread(true);
        forceHandler->waitForThread(true);
        
        delete forceHandler;
        delete physics;
    }
    

    int side;
    void init();


    void start(){
        resetToInit();
        forceHandler->startThread();
        physics->startThread();
    }
    
    
    void stopForces()
    {if(forceHandler!=nullptr){forceHandler->waitForThread(true);}};
    
    void startForces(){forceHandler->startThread();};
    
    
    void initGrid();
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
    ofColor color;
    vector<ofVec3f> sizes;
    typedef std::pair<ofIndexType,ofIndexType>  IndexType;
    vector<IndexType> indexes;
    void setNumParticles(int num);
    void setFPS(int fps){
        deltaT = 1000.0/fps;
    }
    void loadModel(string name = "");
    void update();
    void draw();
    void resetToInit();
    ForceHandler *   forceHandler;
    PhysicsHandler *  physics;
    
    MultiParticleHandler * owner;
    
    
    
    typedef itg::NearestNeighbour<Array<float,COLNUM,1> >  MyNN;
    MyNN  *  nn;
    
    typedef pair<size_t, float> match;
    typedef vector<match> matches;
    vector <matches> activeLias;
};

#endif /* ParticleHandler_hpp */
