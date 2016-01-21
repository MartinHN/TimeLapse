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
    ofScopedLock lock(mutex);
    numParticles = num;
#ifndef FIXEDSIZE
    
    int oldNum = MAX(0,position.rows()-1);
    position.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
    
    for(int i = oldNum; i < num ; i++){
        position.row(i).setZero();
    }
    velocity.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
    //    velocity.setZero();
    acceleration.conservativeResize(num,MyMatrixType::ColsAtCompileTime);
    //    acceleration.setZero();
#endif
#if NEED_TO_CAST_VERT
    floatPos = position.cast<float>();
    vbo.setVertexData((float * )floatPos.data(), MyMatrixType::ColsAtCompileTime,num, GL_DYNAMIC_DRAW);
#else
    vbo.setVertexData((float * )position.data(), MyMatrixType::ColsAtCompileTime,num, GL_DYNAMIC_DRAW);
#endif
#ifdef    DIFFERENT_SIZES
    sizes.resize(num);
    for(int i = 0 ; i < numParticles ; i++){
        sizes[i] = ofVec3f(2000.0/(sqrt(numParticles)));
    }
    vbo.setNormalData(&sizes[0], numParticles, GL_DYNAMIC_DRAW);
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
//    originType = 1;
//    int dumb =originType;
//    changeOrigin(dumb);
#endif
    forceHandler->initForces();
    
}


void ParticleHandler::changeOrigin(int & type){
    
        if(type==0){
                        owner->stopForces();
            initGrid(10000);
            owner->startForces();
            
        }
        
        else{
            ofDirectory d("models");
            d.allowExt("obj");
            d.listDir();
            
            vector<ofFile>  models = d.getFiles();
//            ofLog() << d.getAbsolutePath();
            if(models.size()){
            owner->stopForces();
            loadModel(models[(type - 1)%models.size()]);
            owner->startForces();
            }
        }
    
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


void ParticleHandler::initGrid(int num){
    
    numParticles = num;
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
    setNumParticles(numParticles);
    
}


void ParticleHandler::resetToInit(){
    position = positionInit;
    velocity.setZero();
    acceleration.setZero();
}
void ParticleHandler::initIndexes(){
    
    
    switch (lineStyle) {
            
            
        case 1:{
            nn->buildIndex(positionInit);
            
            int maxNN = 6;
            
            vector<size_t> foundNN;
            vector<float> foundNNDist;
            foundNNDist.resize(maxNN);
            foundNN.resize(maxNN);
            lineIdx.resize(numParticles*maxNN*2);
            
#if NEED_TO_CAST_VERT
            floatPos = positionInit.cast<float>();
            for(int i = 0 ; i < numParticles;i++){
                nn->findPointsWithinRadius(floatPos.row(i), 0.1*getWidthSpace(), activeLias[i]);
#else
                for(int i = 0 ; i < numParticles;i++){
                    nn->findNClosestPoints(positionInit.row(i),maxNN, foundNN, foundNNDist);
                    for(int j = 0 ; j < maxNN ; j++){
                        lineIdx[(i*maxNN + j)*2] = i;
                        lineIdx[(i*maxNN + j)*2+1] =0;//foundNN[j] ;
                        //                ofLog() << i*maxNN*2 + j << "," <<  i <<","<< foundNN[j];
                    }
#endif
                }
            }
        default:
            lineIdx.resize(numParticles);
            for(int i = 0 ; i < numParticles ; i++){
                lineIdx[i] = i;
            }
            break;
            
        }
            vbo.setIndexData(&lineIdx[0], lineIdx.size(), GL_STATIC_DRAW);
    }
    
    
    void ParticleHandler::draw(){
        vbo.draw( GL_POINTS , 0 ,numParticles);
        
        
    }
    
    void ParticleHandler::drawLines(){
        //        vbo.updateIndexData(&lineIdx[0],lineIdx.size());
        if(lineStyle>0){
            
            vbo.drawElements(lineStyle, lineIdx.size());
        }
        else{
                vbo.drawElements(GL_LINES, lineIdx.size());
             
        }
        //        vbo.drawElements(GL_LINE_STRIP, numParticles);
    }
    double ParticleHandler::getWidthSpace(){
        return owner->widthSpace;
    }
    void ParticleHandler::loadModel(ofFile file){
        bool addFaceCenter = true;
//        string path="models/";//"/Users/Tintamar/Work/BO/Chrone/tests/";
//        if(name=="")path+= "pyramid.obj";
//        else path+=name;
//        ofFile file(path);
        ofLog() << "loading 3d " << file.getBaseName();
        file.open(file.getAbsolutePath());
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
        positionInit.conservativeResize(numParticles, MyMatrixType::ColsAtCompileTime);
        
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
        file.close();
        setNumParticles(numParticles);
    }
    
