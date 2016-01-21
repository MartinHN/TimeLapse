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
        params.setName("physics");
        
        CPARAM(fr,.9,0,1);
        CPARAM(maxVel,0,0,10);
        CPARAM(onlyVelocity,true,false,true);
    }
    
    void threadedFunction() override;
    
    void doJob();
    
    double actualDelay;
    
    
    ofParameter<float> fr = .9,maxVel = 10;
    ofParameter<bool> onlyVelocity;
    ofParameterGroup params;
    
    ParticleHandler * owner;
};
#endif /* PhysicsHandler_hpp */
