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
        if(owner->forceHandler!=nullptr)
            owner->forceHandler->doJob();
#endif
        int toWait =owner->deltaT - (ofGetElapsedTimeMillis() - millilast);
        
        if(toWait>0){ofSleepMillis(toWait);};
        if(actualDelay>1.3*owner->deltaT){ofLog() <<"highpressure Physics" << actualDelay;}
        
        //
    }
}


void PhysicsHandler::doJob(){
    double dT = (actualDelay/100.0) * globalTimeFactor*globalTimeFactor;
    double _widthSpace =owner->getWidthSpace();
    if(onlyVelocity){
#ifdef SPLIT_THREAD
        ofScopedLock(owner->mutex);
#endif
        if(maxVel!=0){
            Array<MatReal,Dynamic,1> norms = owner->acceleration.matrix().rowwise().norm();
            //        norms.cwiseEqual(0);// * norms;
            Array<MatReal,Dynamic,1> normsI = norms.max(0.000000001).cwiseInverse();
            norms = norms.min(maxVel*_widthSpace/dT) * normsI;
            owner->acceleration  = owner->acceleration.colwise() * norms;
            //        ofLog() << tmpVel;
            //            owner->acceleration= tmpVel;
        }
        owner->position+=owner->acceleration*dT;
    }
    else{
        {
#ifdef SPLIT_THREAD
            ofScopedLock(owner->mutex);
#endif
      
            owner->velocity+=owner->acceleration * dT;
        }
        if(fr!=1)
            owner->velocity*=pow((double)fr,dT);
        if(maxVel!=0){
            Array<MatReal,Dynamic,1> norms = owner->velocity.matrix().rowwise().norm();
            //        norms.cwiseEqual(0);// * norms;
            Array<MatReal,Dynamic,1> normsI = norms.max(0.00000001).cwiseInverse();
            norms = norms.min(maxVel*_widthSpace/dT) * normsI;
            owner->velocity  = owner->velocity.colwise() * norms;
            //        ofLog() << tmpVel;
            //            owner->velocity= tmpVel;
        }
        owner->position+=owner->velocity *   dT;
        
        if(wrapSpace){
        for(int i = 0 ; i < owner->position.rows() ; i ++){
            double overFlow = owner->position.row(i)[0];
            if(overFlow<-_widthSpace){
                owner->position.row(i)[0] += 2*_widthSpace;
            }
            if(overFlow>_widthSpace){
                owner->position.row(i)[0] -= 2*_widthSpace;
                
            }
        }
        }
    }
    
    
};