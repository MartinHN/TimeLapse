//
//  PhysicsHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#include "PhysicsHandler.hpp"
#include "ParticleHandler.hpp"

void PhysicsHandler::threadedFunction() {
    uint64_t millilast=ofGetElapsedTimeMillis();
    while(isThreadRunning()){
        
        actualDelay = ofGetElapsedTimeMillis() - millilast;
        //        actualDelay = owner->deltaT;
        millilast = ofGetElapsedTimeMillis();
        doJob();
#ifndef  SPLIT_THREAD
        owner->forceHandler->doJob();
#endif
        int toWait =owner->deltaT - (ofGetElapsedTimeMillis() - millilast);

        if(toWait>0){ofSleepMillis(toWait);};
        if(actualDelay>1.3*owner->deltaT){ofLog() <<"highpressure Physics" << actualDelay;}
        
        //
    }
}


void PhysicsHandler::doJob(){
    float dT = (actualDelay/100.0) * globalTimeFactor*globalTimeFactor;
    if(onlyVelocity){
        ofScopedLock(owner->mutex);
        owner->position+=owner->acceleration*dT;
    }
    else{
        {
            ofScopedLock(owner->mutex);
            owner->velocity+=owner->acceleration * dT;
        }
        if(fr!=1)
            owner->velocity*=pow(fr,dT);
        if(maxVel!=0){
            Array<MatReal,Dynamic,1> norms = owner->velocity.matrix().rowwise().stableNorm();
            //        norms.cwiseEqual(0);// * norms;
            Array<MatReal,Dynamic,1> normsI = norms.max(0.00001).cwiseInverse();
            norms = norms.min(maxVel*owner->getWidthSpace()/dT) * normsI;
            MyMatrixType tmpVel  = owner->velocity.colwise() * norms;
            //        ofLog() << tmpVel;
            owner->velocity= tmpVel;
        }
        owner->position+=owner->velocity *   dT;
    }
    
    
};