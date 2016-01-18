//
//  AppViz.cpp
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#include "AppViz.h"


#include "ofApp.h"

vector<float> & AppViz::getHPCP(bool smooth){
    if(app){return smooth?HPCP_s:app->HPCP;}
    else{ofLog() << "no HPCP" ;}
}
//    else{return vector<float>(0)}
vector<float> & AppViz::getMFCC(bool smooth){    if(app){return smooth?MFCC_s:app->MFCC;}}
int & AppViz::getcluster(){ if(app){return app->cluster;}};

vector<float> & AppViz::getEnv(bool smooth) {if(app){return smooth?enveloppes_s:app->enveloppes;}}
bool AppViz::hasEnv(){return app?app->enveloppes.size():false;}
bool AppViz::hasMFCC(){
    return app? app->MFCC.size()>0:false;
};
bool AppViz::hasHPCP(){
    return app? app->HPCP.size()>0:false;
};


float AppViz::getHeight(){
    return app?app->heightOut:800;
}

float AppViz::getWidth(){
    return app?app->widthOut:1280;
}