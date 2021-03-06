//
//  PhysicsHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#ifndef PhysicsHandler_hpp
#define PhysicsHandler_hpp

#include <stdio.h>

#include "ofMain.h"


#include "ConfigParticles.h"

class ParticleHandler;




class PhysicsHandler  : public ofThread{
    public :
    
    PhysicsHandler(ParticleHandler * p):owner(p){
        params = new ofParameterGroup();
        params->setName("physics");
        
        CPARAM(fr,.9,0,1);
        CPARAM(maxVel,0,0,.1);
        CPARAM(consistency,0,0,1);
        CPARAM(onlyVelocity,false,false,true);
        CPARAM(globalTimeFactor, 1,0.000001,5);
        CPARAM(wrapSpace,false,false,true);
    }
    ~PhysicsHandler(){
        delete params;
    }
    
    void threadedFunction() override;
    
    void doJob();
    
    double actualDelay;
    
    
    ofParameter<float> fr = .9,maxVel = 10,globalTimeFactor,consistency;
    ofParameter<bool> onlyVelocity,wrapSpace;

    ofParameterGroup * params;
    
    ParticleHandler * owner;
};
#endif /* PhysicsHandler_hpp */
