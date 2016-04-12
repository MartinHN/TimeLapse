//
//  ForceHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#include "ForceHandler.hpp"


#include "ParticleHandler.hpp"




void ForceHandler::threadedFunction(){
    while(isThreadRunning()){
        uint64_t micro = ofGetElapsedTimeMillis();
        doJob();
        int toWait = owner->deltaT - (ofGetElapsedTimeMillis() - micro);
        if(toWait>0){
            //                    ofLog() <<"waiting F" << toWait;
            ofSleepMillis(toWait);
        };
    }
}
void ForceHandler::preComputeDists(){
    {
        ofScopedLock lk(mutex);
        attractors = asyncAttractors;
    }
    for( auto & a:attractors){
        Array<MatReal,1,MyMatrixType::ColsAtCompileTime> pos;
        pos(0) = a.second.x;
        pos(1) = a.second.y;
#if COLNUM >2
        pos(2) = a.second.z;
#endif
        attrVec[a.first] = owner->position.array().rowwise() - pos;
        attrNorm[a.first] = attrVec[a.first].matrix().rowwise().stableNorm();
    }
}

void ForceHandler::doJob(){

    ofScopedLock(owner->mutex);
    owner->acceleration.setZero();

    preComputeDists();
    if(owner->position.rows()>0){
        for(auto k :availableForces){
            if(k.second->isActive){
                ofScopedLock lk(mutex);
                k.second->updateForce();
            }


        }
    }

}

ForceHandler::Force::Force(ForceHandler * f,string name):forceOwner(f){

    isActive = false;
    forceOwner->availableForces[name] = this;
    params = new ofParameterGroup();
    params->setName(name);
    isActive.set("isActive",true,false,true);

    //    CPARAM(*isActive,true,false,true);
    forceOwner->forcesParams->add(*params);

}
void ForceHandler::Force::linkParams(){

}
void ForceHandler::activateForce(string name,bool t){
    //    if(t){
    ForceIt f = availableForces.find(name);
    if(f!=availableForces.end()){
        availableForces[name]->isActive = t;// = f->second;
    }

    //    }
    //    else{
    //         ForceIt f = activeForces.find(name);
    //        if(f!=activeForces.end()){
    //            activeForces.erase(name);
    //        }
    //        else activateForce(name);
    //    }
}

void ForceHandler::setAttractor(const string & name,ofVec3f pos){
    //ofScopedLock lk(mutex);
    asyncAttractors[name] = pos;
}
void ForceHandler::removeAttractor(const string & name){
    ofScopedLock lk(mutex);
    asyncAttractors.erase(name);
}
void ForceHandler::clearAttractors(){
    ofScopedLock lk(mutex);
    asyncAttractors.clear();
}
void ForceHandler::resetAttractors(map<string,ofVec3f> & m){
    ofScopedLock lk(mutex);
    asyncAttractors = m;
}

class Origin : public ForceHandler::Force{
public:
    Origin(ForceHandler * f):Force(f,"origin"){


    }
    void linkParams()override{
        CPARAM(k,0.1,0,0.5);
    }
    //    MyMatrixType origins;

    ofParameter<float> k=0.1;
    void changeNumParticles(int num) override{

    }


    void updateForce()override{
        FORCE_OWNER->acceleration += k*( FORCE_OWNER->positionInit-FORCE_OWNER->position );

    }
};

class Spring:public ForceHandler::Force{
public:
    Spring(ForceHandler * f):ForceHandler::Force(f,"spring"){

    }
    void linkParams ()override{
        CPARAM(k,.1,0,10);
        CPARAM(l0,.1,0,1);
        CPARAM(rMin,0,0,1);
        CPARAM(rMax,0,0,1);

        CPARAM(perAttr,false,false,true);
        CPARAM(numPerAttr,0,0,100);
        CPARAM(power,1,-3,3);
    }
    //    Matrix<MatReal,1,3> pos;
    ofParameter<float> k,l0,rMax,rMin;
    ofParameter<int> numPerAttr;
    ofParameter<MatReal>power;
    ofParameter<bool> perAttr;
    Matrix<MatReal,Dynamic,1> rMinMaxMask;
    void updateForce()override{
        double _l0 = l0 * FORCE_OWNER->getWidthSpace();
        double _rMax = rMax * FORCE_OWNER->getWidthSpace();
        double _rMin = rMin * FORCE_OWNER->getWidthSpace();
        int idx = 0;

        int step = FORCE_OWNER->numParticles /(perAttr.get()? MAX(forceOwner->attractors.size(),1)+1:1);
        if(numPerAttr>0){
            step = numPerAttr;
        }
        rMinMaxMask.resize(step,1);
        forceOwner->buf1D.resize(step, 1);
        forceOwner->buf3D.resize(step,3);

        for(auto & f:forceOwner->attractors){
            forceOwner->buf1D  =   forceOwner->attrNorm[f.first].block(idx,0,step,1).col(0);
            //            if(_rMax>0 && _rMax>_rMin){
            //                rMinMaxMask=forceOwner->buf1D.cwiseMin(_rMax).cwiseEqual(_rMax).cast<MatReal>();
            //                rMinMaxMask = rMinMaxMask.array().cwiseProduct(forceOwner->buf1D.cwiseMax(_rMin).cwiseEqual(_rMin).cast<MatReal>().array());
            //            }

            forceOwner->buf1D-=_l0;

            //            if(power!=1){
            //                for(int k = 1 ; k< forceOwner->buf1D.rows() ; k++){
            //                    forceOwner->buf1D.row(k)[0]=std::pow(forceOwner->buf1D.row(k)[0],power);
            //                }
            //                if(power<0){
            //                    forceOwner->buf1D=forceOwner->buf1D.cwiseInverse();
            //                }
            //            }
            forceOwner->buf1D*=-k;
            //            forceOwner->buf1D=forceOwner->buf1D.cwiseProduct(rMinMaxMask.array());
            forceOwner->buf3D =  forceOwner->attrVec[f.first].block(idx,0,step,COLNUM).eval();
            forceOwner->buf3D= forceOwner->buf3D.colwise()/(forceOwner->buf3D.matrix().rowwise().stableNorm().array());
            FORCE_OWNER->acceleration.block(idx,0,step,COLNUM)+=  forceOwner->buf3D.colwise() * forceOwner->buf1D;
            if(numPerAttr>0 || perAttr.get()){
                idx+=step;
            }


        }
    }
};
class Rotate:public ForceHandler::Force{
public:
    Rotate(ForceHandler * f):ForceHandler::Force(f,"rotate"){
        tmp = nullptr;
    }
    void linkParams()override{
        CPARAM(kk,0.0,0.0,10.0);
    }

    ofParameter<float> kk = .001;
    MyMatrixType *   tmp ;

    void changeNumParticles(int num) override{
        if(tmp==nullptr)tmp = new MyMatrixType();
#ifndef FIXEDSIZE
        tmp->resize(num,MyMatrixType::ColsAtCompileTime);
#endif
    }



    void updateForce()override{
        for(auto & f:forceOwner->attractors){


            MatReal *      data=   FORCE_OWNER->acceleration.data();
            MatReal *      cpos=   FORCE_OWNER->position.data();


            int idx = 0;
            float r = 10,theta = .1,ctheta2 = cos(theta)*cos(theta),stheta2 = sin(theta)*sin(theta);
            ofVec3f d,dp ;

            for(int i = 0 ; i < FORCE_OWNER->numParticles ; i++){
                int idx = i*COLNUM;
                d.set( cpos[idx] -f.second.x,cpos[idx+1] -f.second.y,cpos[idx+2] -f.second.z);
                d.normalize();
                dp.set(d[1],d[0],-d[0]*d[1]);
                dp.normalize();
                d.cross(dp);
                d*=kk;
                //            d.normalize();
                data[idx]+= d[0];
                data[idx+1] += d[1];
#if COLNUM >2
                data[idx+2] += d[2];
#endif
            }
            //            }
        }
    }

};


class Neighbors:public ForceHandler::Force{
public:
    Neighbors(ForceHandler * f):ForceHandler::Force(f,"neighbors"){

        //                initDist.addListener(this , & Neighbors::initDistances);
    }

    void linkParams() override{
        CPARAM(k,0.0002,0,0.3);
        CPARAM(distMax,0,0,0.2);
        CPARAM(ripMax,0,0,0.2);
        CPARAM(l0,1,0,5);
        CPARAM(propa,0,0,1);
        CPARAM(initDist,false,true,false);
    }
    ofParameter<float> k,distMax,ripMax,l0 ,propa;


    vector<unsigned int> myLineIdx;
    Array<MatReal,Dynamic,COLNUM> lastDists;
    ofParameter<bool> initDist;

    ofMutex mumu;
    void changeNumParticles(int num) override{
        ofScopedLock lk(mumu);
        myLineIdx = FORCE_OWNER->lineIdx;
        initDist = true;

    }


    void initDistances(){

        ofScopedLock lk(mumu);
        lastDists.resize(myLineIdx.size()/2, COLNUM);

        MatReal tmpNorm;
        for(int i = 0 ; i <MIN(myLineIdx.size()-1,2*lastDists.rows())  ; i+=2){
            lastDists.row(i/2)  = (FORCE_OWNER->positionInit.row(myLineIdx[i]) -FORCE_OWNER->positionInit.row(myLineIdx[i+1]));
            //            tmpNorm = lastDists.row(i/2).matrix().norm();
            //            if(tmpNorm == 0)
            //                lastDists.row(i/2).setZero();
            //            else{
            //                lastDists.row(i/2)/=tmpNorm;
            //            }

        }
        initDist = false;
        ofSleepMillis(1000);

    }
    void updateForce()override{

        if(initDist){
            initDistances();
        }

        double _ripMax = ripMax*FORCE_OWNER->getWidthSpace();

        ofScopedLock lk(mumu);
        //        if(lastDists.rows()<1 ||  myLineIdx.size()<2)return;
        for(int i = 0 ; i <std::min((int)myLineIdx.size()-1,(int)lastDists.rows()*2)  ; i+=2){
            Array<MatReal,COLNUM,1> dist = FORCE_OWNER->position.row(myLineIdx[i]) -FORCE_OWNER->position.row(myLineIdx[i+1]);
            MatReal norm = dist.matrix().norm();
            if(norm>0 && (_ripMax==0 ||norm<_ripMax)){
                MatReal _l0 = FORCE_OWNER->lineDists[i/2] * l0;
                double coef = (norm-_l0);
                if(distMax>0){
                    coef = std::min((float)abs(coef),(float)(distMax*FORCE_OWNER->getWidthSpace()));
                }
                double ksign = k*coef / norm ;//(coef>0?k.get():-k.get())
                                              //                dist*=ksign;
                dist = dist*ksign + propa*(dist - lastDists.row(i/2).transpose());



                FORCE_OWNER->acceleration.row(myLineIdx[i])-=dist;
                FORCE_OWNER->acceleration.row(myLineIdx[i+1])+=dist;



            }
        }



    }

};


class Noise:public ForceHandler::Force{
    public :
    Noise(ForceHandler * f):ForceHandler::Force(f,"Noise"){

        index=0;
    }

    void linkParams()override{
        CPARAM(strength,0,0,10);
        CPARAM(spread,.1,0,1);
        CPARAM(speed,1,0,100);
        lastSpread= spread;
    }
    Array<MatReal,Dynamic,COLNUM> noiseMat;
    float lastSpread;
    void changeNumParticles(int num) override{
        initNoiseMat();
    }
    void initNoiseMat(){
        noiseMat.resize(FORCE_OWNER->positionInit.rows(), COLNUM);
        index = 0;
        for(int i = 0 ; i < FORCE_OWNER->positionInit.rows() ; i++){
            ofVec3f noise = ofVec3f(ofSignedNoise((spread/FORCE_OWNER->getWidthSpace())*FORCE_OWNER->positionInit.row(i)[0]),
                                    ofSignedNoise((spread/FORCE_OWNER->getWidthSpace())*FORCE_OWNER->positionInit.row(i)[1])
#if COLNUM==3
                                    ,ofSignedNoise((spread/FORCE_OWNER->getWidthSpace())*FORCE_OWNER->positionInit.row(i)[2]));
#else
            );
#endif
            noiseMat.row(i)[0] = noise.x;
            noiseMat.row(i)[1] = noise.y;
#if COLNUM==3
            noiseMat.row(i)[2] = noise.z;
#endif
        }

    }


    void updateForce()override{
        if(lastSpread!=spread){
            initNoiseMat();
            lastSpread= spread;
        }
        while((int)index>= FORCE_OWNER->acceleration.rows()){
            index-=FORCE_OWNER->acceleration.rows();
        }

        int startIdx = index;
        int toCopy = FORCE_OWNER->acceleration.rows() - startIdx;
        FORCE_OWNER->acceleration.block(startIdx,0,toCopy,COLNUM)+= strength*(noiseMat).block(0,0,toCopy,COLNUM);
        FORCE_OWNER->acceleration.block(0,0,startIdx,COLNUM)+= strength*(noiseMat).block(toCopy,0,startIdx,COLNUM);
        index+=speed;


    }
    float index;
    ofParameter<float> strength,spread,speed;

};




class Tornado: public ForceHandler::Force{
public:
    Tornado(ForceHandler * f):ForceHandler::Force(f,"Tornado"){

    }
    ofParameter<ofVec2f> center;
    ofParameter<float> radius,noise,attract,speed,noiseSpread;

    void linkParams()override{
        CPARAM(center,ofVec2f(0),ofVec2f(-1),ofVec2f(1));
        CPARAM(radius,0.25,0,0.5);
        CPARAM(attract,0.,0.,1.);
        CPARAM(speed,0.,0.,1.);
        CPARAM(noise,0,0,10);
        CPARAM(noiseSpread,0,0,10);
    }
    void changeNumParticles(int num) override{


    }

    void updateForce()override{

        //        forceOwner->buf3D.resize(FORCE_OWNER->position.rows(), COLNUM);
        for(int i = 0 ; i < FORCE_OWNER->position.rows() ; i++){
            ofVec2f attractDir (FORCE_OWNER->position.row(i)[0] - center->x*FORCE_OWNER->getWidthSpace(),
                                FORCE_OWNER->position.row(i)[1] - center->y*FORCE_OWNER->getWidthSpace());


            ofVec2f tangent(attractDir.y,-attractDir.x);
            float norm = attractDir.length()-radius*FORCE_OWNER->getWidthSpace();
            if(noise>0){norm*=noise*ofNoise( FORCE_OWNER->position.row(i)[2]/(noiseSpread*FORCE_OWNER->getWidthSpace()));}
            attractDir= attractDir.getNormalized()*norm;

            ofVec3f force = -attractDir*attract + tangent.getNormalized()*speed;

            FORCE_OWNER->acceleration.row(i)[0] += force.x;
            FORCE_OWNER->acceleration.row(i)[1] += force.y;

        }

    }

};

class Flock:public ForceHandler::Force{
public:
    Flock(ForceHandler* f):ForceHandler::Force(f,"Flock"){
        nn = make_shared<MyNN>();

        CPARAM(minDist,.01,0,.3);
        CPARAM(attrRepulse,0.5,0,1);
        CPARAM(k,.01,0,.3);
        CPARAM(resolution,10,1,1000);
        CPARAM(maxPointsPerFlock,10,2,50);
    }

    ofParameter<int> resolution,maxPointsPerFlock;
    ofParameter<float>minDist,k,attrRepulse;
    typedef Eigen::Array<double, 3, 1, 0, 3, 1>  pointType;
    vector<int> classes;
    float _minDist;
    int startIdx = 0;
    void updateForce()override{
        buildNN();
        _minDist =minDist*FORCE_OWNER->getWidthSpace();
        startIdx++;
        startIdx%=FORCE_OWNER->position.rows();
        for(int ii = 0 ; ii  < FORCE_OWNER->position.rows() ; ii+=resolution){
            int i = (ii+startIdx)% FORCE_OWNER->position.rows();
            Eigen::Array<float, 3, 1, 0, 3, 1> point = FORCE_OWNER->position.row(i).cast<float>();
            vector<size_t> indices;
            vector<float> dists;

            nn->findNClosestPoints(point, maxPointsPerFlock, indices, dists);

            for(int j = indices.size() -1 ; j>=0 ;--j){
                int di = indices[j];
                float dlen = sqrt(dists[j]);
                if(dlen<_minDist){
                    if(dlen>0.00001){
                        float norm = (dlen- attrRepulse*_minDist)*k;
                        pointType d (FORCE_OWNER->position.row(di) - FORCE_OWNER->position.row(i));
                        d/=dlen;
                        FORCE_OWNER->acceleration.row(di)-= norm*d;
                        FORCE_OWNER->acceleration.row(i)+= norm*d;
                    }
//                }
            }
            }

        }

    }
    void changeNumParticles(int num) override{
        classes.resize(num);
    }

    void buildNN(){
        nn->buildIndex(FORCE_OWNER->position);

    }
    typedef itg::NearestNeighbour<Array<float,COLNUM,1> >  MyNN;
    shared_ptr<MyNN>  nn;

};



void ForceHandler::initForces(){
    new Origin(this);

    new Rotate(this);
    new Spring(this);
    new Tornado(this);
    new Noise(this);
    //    activateForce("rotate");
    new Neighbors(this);
    new Flock(this);
    
    //    activateForce("neighbors");
    
    for(auto & t:availableForces){
        t.second->params->add(t.second->isActive);
        t.second->linkParams();
        
    }
    
    changeNumParticles(owner->numParticles);
}



void ForceHandler::drawAttractors(){
    ofSetColor(255,0,0,255);
    for(auto a:asyncAttractors){
        ofNoFill();
        ofDrawCircle(a.second, owner->getWidthSpace()/20);
        ofFill();
        ofDrawBitmapString( a.first,a.second);
    }
}