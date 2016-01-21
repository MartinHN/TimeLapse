//
//  MultiParticleHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 14/01/2016.
//
//

#include "MultiParticleHandler.hpp"
#include "ofApp.h"

void MultiParticleHandler::initGL(){
    ofSetLogLevel("ofShader", ofLogLevel::OF_LOG_VERBOSE);
    
    ofDisableArbTex();
    //        ofSetVerticalSync(false);
    ofLoadImage(pointTexture, "shaders/dot.png");
    ofEnableArbTex();
    renderOnGPU(false);
    
    widthSpace = 100;
    distortFactor = .1;
    //    ofEnablePointSprites();
    cam.setDistance((widthSpace/2)*(1+1.0/distortFactor));
    cam.setFov(ofRadToDeg(2*atan(distortFactor)));
    cam.setNearClip(0.001);
    cam.setFarClip((widthSpace/2) *  2000);
    
    cam.orbit(0, 0, cam.getDistance());
    
}

void MultiParticleHandler::update(){
    cam.setDistance((widthSpace/2)*(1+1.0/distortFactor)/zoom);
    cam.setFov(ofRadToDeg(2*atan(distortFactor)));
    
    clearAttractors();
    map<string,ofVec3f> attrUp = app->attractors;
    for(auto & a:app->attractors){
        int num = ofToInt(a.first);
        if(num>1)num = -1;
        ofVec3f v =normalizedToWorld(a.second);
//        ofLog()<<num << ","<<v << "," <<a.second;
        setAttractor(a.first,v,num);
    }
        update(-1);
    
}

void MultiParticleHandler::renderOnGPU(bool t){
    isRenderingOnGPU = t;
    if(t){
        shader.load("shaders/shaderdot");
    }
    else{
        shader.load("shaders/shadertex");
    }
    
    
}



ofVec3f MultiParticleHandler::normalizedToWorld(ofVec3f v){
    // transform v.z from world to camera coordinates
    float zFar = cam.getFarClip();
    float zNear = cam.getNearClip();
    
    double C = -(zFar+zNear)/(zFar-zNear);
    double D = -2.0*zFar*zNear/(zFar-zNear);
    float z = cam.getDistance() - v.z;
    
    //avoid nans
    if(z!=0)v.z = -C-D/-z;
    else v.z = -99999;
    
    
    v.x= v.x*2 - 1;
    v.y= v.y*2 - 1;
    //get inverse camera matrix
    ofMatrix4x4 inverseCamera;
    inverseCamera.makeInvertOf(cam.getModelViewProjectionMatrix());
    
    //convert camera to world
    return v * inverseCamera;
}



ofVec3f MultiParticleHandler::screenToWorld(ofVec3f v){
    
    // transform v.z from world to camera coordinates
    float zFar = cam.getFarClip();
    float zNear = cam.getNearClip();
    
    double C = -(zFar+zNear)/(zFar-zNear);
    double D = -2.0*zFar*zNear/(zFar-zNear);
    float z = cam.getDistance() - v.z;
    
    //avoid nans
    if(z!=0)v.z = -C-D/-z;
    else v.z = -99999;
    
    return cam.screenToWorld(v);
    
}

void MultiParticleHandler::draw(){
    
    
    glDepthMask(GL_FALSE);
    ofEnablePointSprites();
    ofEnableSmoothing();
    
//        ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofEnableAlphaBlending();
    glPointSize(pointSize);
    glLineWidth(lineWidth);
    ofDisableDepthTest();
    cam.begin();
    shader.begin();
    
    
    
//        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    //    ofSetColor(50,50,50,10);
    if(!isRenderingOnGPU)
        pointTexture.bind();
    for(auto &f:particlesList){
        ofSetColor(f->color,alphaGlobal);
        f->draw();
    }
    
    if(!isRenderingOnGPU)
        pointTexture.unbind();
    
    
    
    shader.end();
    
    for(auto &f:particlesList){
        ofSetColor(f->color,alphaGlobal);
        f->drawLines();
    }
    
    for(auto &f:particlesList){
    f->forceHandler->drawAttractors();
    }
    cam.end();
    
    glDepthMask(GL_TRUE);
    ofDisablePointSprites();
}
