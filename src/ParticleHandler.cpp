//
//  ParticleHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#include "ParticleHandler.hpp"
#include "MultiParticleHandler.hpp"


void ParticleHandler::setNumParticles(int num){
    //    ofScopedLock lock(mutex);
    numParticles = num;
#ifndef FIXEDSIZE
    position.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
    velocity.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
    acceleration.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
#endif
#if NEED_TO_CAST_VERT
    floatPos = position.cast<float>();
    vbo.setVertexData((float * )floatPos.data(), MyMatrixType::ColsAtCompileTime,num, GL_STATIC_DRAW);
#else
    vbo.setVertexData((float * )position.data(), MyMatrixType::ColsAtCompileTime,num, GL_STATIC_DRAW);
#endif
#ifdef    DIFFERENT_SIZES
    sizes.resize(num);
    for(int i = 0 ; i < numParticles ; i++){
        sizes[i] = ofVec3f(2000.0/(sqrt(numParticles)));
    }
    vbo.setNormalData(&sizes[0], numParticles, GL_STATIC_DRAW);
#endif
    
    initIndexes();
    forceHandler->changeNumParticles(num);
    
}

void ParticleHandler::init(){
    

#ifdef FIXEDSIZE
    setNumParticles(FIXEDSIZE);
#else
//    setNumParticles(10000);
//    initGrid();
    loadModel();

#endif



    forceHandler->initForces();
    
}
void ParticleHandler::update(){
#if NEED_TO_CAST_VERT
    floatPos = position.cast<float>();
    vbo.updateVertexData((float*)floatPos.data(),numParticles);
#else
    vbo.updateVertexData((float*)position.data(),numParticles);
#endif
    
#ifdef DIFFERENT_SIZES
//    vbo.updateNormalData(&sizes[0], numParticles);
#endif
    
}


void ParticleHandler::initGrid(){
    
    
    side = pow((double)numParticles,0.334);
    ofVec3f steps(getWidthSpace()/side);
    positionInit.resize(numParticles,MyMatrixType::ColsAtCompileTime);
    for(int i = 0  ; i < numParticles ; i++){
        MatReal ii = (i%side) - side/2.0  +0.5;
        MatReal jj = (i/side)%(side) - side/2.0 + 0.5 ;
        MatReal kk = (int)(i/(side*side)) - side/2.0 +0.5;
        positionInit.row(i)[0] = ii*steps.x;
        positionInit.row(i)[1] = jj*steps.y;
#if COLNUM > 2
        positionInit.row(i)[2] = kk*steps.z;
#endif
    }
    
    //    int side = pow(numParticles,0.5);
    //    ofVec2f steps(widthSpace/side);
    //    for(int i = 0  ; i < numParticles ; i++){
    //        MatReal ii = (i%side) - side/2 ;
    //        MatReal jj = (int)(i/side) - side/2 ;
    //
    //        position.row(i)[0] = ii*steps.x;
    //        position.row(i)[1] = jj*steps.y;
    //        position.row(i)[2] = 0;
    //    }
    resetToInit();
    
}


void ParticleHandler::resetToInit(){
    position = positionInit;
    velocity.setZero();
    acceleration.setZero();
}
void ParticleHandler::initIndexes(){
//    IndexType b;
//    //    for(int i = 0 ; i < numParticles/2 ;i++){
//    //        b.first = i*2;
//    //        b.second = (i+1)*2;
//    //        indexes.push_back(b);
//    //    }
//    for(int i = 0 ; i < side ; i++){
//        for(int j = 0 ; j < side ; j++){
//            for(int k =  0; k < side ; k++){
//                int num = i*side*side + j*side+k;
//                b.first = num;
//                //                b.second = num-1;
//                //                indexes.push_back(b);
//                if(k!=side-1){
//                    b.second = num+1;
//                    indexes.push_back(b);
//                }
//                if(j!=side-1){
//                    b.second = num+side;
//                    indexes.push_back(b);
//                    //                b.second = num-side;
//                    //                indexes.push_back(b);
//                }
//                if(i!=side-1){
//                    b.second = num+side*side;
//                    //                indexes.push_back(b);
//                    //                b.second = num-side*side;
//                    indexes.push_back(b);
//                }
//                
//            }
//        }
//    }
    
    
    nn->buildIndex(positionInit);
    activeLias.resize(numParticles);
#if NEED_TO_CAST_VERT
            floatPos = positionInit.cast<float>();
    for(int i = 0 ; i < numParticles;i++){
        nn->findPointsWithinRadius(floatPos.row(i), 0.051*getWidthSpace(), activeLias[i]);
#else
        for(int i = 0 ; i < numParticles;i++){
            nn->findPointsWithinRadius(positionInit.row(i), 0.051*getWidthSpace(), activeLias[i]);
#endif
    }
    for(auto& l : activeLias){
        if(l.size()>6){
            l.resize(6);
        }
    }
    indexes.clear();
    int idx = 0;
    for(auto &l:activeLias){
    IndexType b;
        b.first = idx;
        for(auto &ll:l){
            b.second = ll.first;
            indexes.push_back(b);
        }
        idx++;
    }
    
    vbo.setIndexData(&indexes[0].first, indexes.size()*2, GL_STATIC_DRAW);
}


void ParticleHandler::draw(){
    vbo.draw( GL_POINTS , 0 ,numParticles);
    vbo.drawElements(GL_LINES, indexes.size()*2);
}


double ParticleHandler::getWidthSpace(){
    return owner->widthSpace;
}
void ParticleHandler::loadModel(string name){
    bool addFaceCenter = true;
    string path="models/";//"/Users/Tintamar/Work/BO/Chrone/tests/";
    if(name=="")path+= "pyramid.obj";
    else path+=name;
    ofFile file(path);
    ofLog() << "loading 3d " << name;
    ofBuffer buf= file.readToBuffer();
    ofBuffer::Lines lines = buf.getLines();
    int numVec = 0,numFace=0;
    for(auto l:lines){
        if(l[0] == 'v'){
            numVec++;
        }
        if (l[0] == 'f'){
            numFace++;
        }
    }
    if(numVec==0 )return;
    numParticles = numVec+(addFaceCenter?numFace:0);
    side = pow((double)numParticles,0.334);
    positionInit.resize(numParticles, MyMatrixType::ColsAtCompileTime);

    int idx=0;
    double wi = getWidthSpace()/2.0;
    for(auto l:lines){
        if(l[0] == 'v'){
            vector<string> ll = ofSplitString(l, " ");
            positionInit.row(idx)[0] = ofToFloat(ll[1])*wi;
            positionInit.row(idx)[1] = ofToFloat(ll[2])*wi;
            positionInit.row(idx)[2] = ofToFloat(ll[3])*wi;
            idx++;
        }
        if(l[0]=='f' && addFaceCenter){
            vector<string> ll = ofSplitString(l, " ");
            int numCoins = ll.size()-1;
            Array<MatReal,3,1> middle;
            middle.setZero();
            for(int i = 0 ; i < numCoins ; i++){
                middle+=positionInit.row(ofToInt(ll[i+1]));
            }
            middle/=numCoins;
            positionInit.row(idx) = middle;
            idx++;
        }
        

        
    }

    setNumParticles(numParticles);
}

