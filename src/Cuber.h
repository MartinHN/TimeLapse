//
//  Cuber.h
//  TimeLapse
//
//  Created by Atila on 03/02/16.
//
//

#ifndef TimeLapse_Cuber_h
#define TimeLapse_Cuber_h
#include "AppViz.h"

class Cuber:public AppViz{
public:
    Cuber(ofApp *a):AppViz(a){
        params.setName("Cuber");
        
        CPARAM(resolution,ofVec2f(4),ofVec2f(0),ofVec2f(100));
        
//        resolution.addListener(this,&Cuber::initGrid);
        initRoot();
        
    }
    
    ofMesh root;
    vector<ofMesh> elements;
    ofLight light;
    vector< vector<float > > rotations;
    ofEasyCam cam;
    ofParameter<ofVec2f> resolution;
    ofMaterial material;
    
    
    
    void update() override{
        for(auto & col:rotations){
            for(auto & el:col){
                el+=1;

            }
        }
    }
    
    void initRoot(){
//        ofEnableArbTex();
        root = ofMesh::box(1,1,1);
//        root = ofMesh::sphere(.5, 10);
        ofLog() <<"normals? : " << root.usingNormals();
        
        for( int i = 0; i < root.getNormals().size(); i++){
            root.setNormal(i,-root.getNormals()[i]);
        }
        cam.orbit(0,0,getWidth());
        cam.setDistance(getWidth());

        cam.setFarClip(10000);

        cam.setNearClip(1);
        light.orbit(ofDegToRad(50), ofDegToRad(50), cam.getDistance());
        light.lookAt(ofVec3f(0));
        light.setPointLight();
        light.setDirectional();
        light.enable();
//        light.setSpecularColor(ofColor::green);
        material.setAmbientColor(ofColor::white);
        material.setSpecularColor(ofColor::red);
    }
    
    
    void draw() override{
        ofVec2f v ((int) resolution->x,(int) resolution->y);
        if(v.x>0 && v.y>0){
            ofEnableArbTex();
//            ofPushStyle();
            cam.begin();
            ofEnableLighting();
            ofEnableDepthTest();
            ofSetSmoothLighting(true);

//            root.draw();
            light.enable();
            material.begin();
            ofVec2f step (getWidth()/v.x,getHeight()/v.y);
            ofQuaternion quat;
            rotations.resize(v.x);
            for(int i =  0 ; i < v.x ; i++){
                rotations[i].resize(v.y);
            }
            for(int i = 0 ; i < v.x ; i ++){
                for (int j =0; j < v.y; j++){
//                    ofPushMatrix();
                    ofPushView();
                    ofTranslate((i - v.x/2)*step.x, (j-v.y/2)*step.y, -50);
                    ofScale(step.x,step.y,step.y);
                    ofRotate(rotations[i][j],1,1,1);

                    root.draw();
                    ofPopView();
//                    ofPopMatrix();
                }
            }
            material.end();
//            ofPopStyle();
//            ofPopView();
            ofDisableDepthTest();
            ofDisableLighting();
            cam.end();
        }
        
        
    }
};
#endif
