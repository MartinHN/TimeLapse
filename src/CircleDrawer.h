//
//  CircleDrawer.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef Hey_CircleDrawer_h
#define Hey_CircleDrawer_h


#include "AppViz.h"
#include "ofApp.h"

class CircleDrawer : public AppViz{
    
    
public:
    CircleDrawer(ofApp* a):AppViz(a){
        rMult.set("mult",1,0,5);
        params.add(rMult);


        params.setName("CircleDrawer");
        CPARAM(X,0.5,0,1);
        CPARAM(Y,0.5,0,5);
        CPARAM(type,0,0,1);
        
    };
    ~CircleDrawer(){};
    
    
    vector <float > sizes = { .5 , .7 ,.8};
    ofParameter<float> rMult;
    ofParameter<float> X,Y;
    
    ofParameter<int>type;
    
    float r;
    
    void draw() override{
        
        
        switch (type){
            case 0:
                if(getcluster()<sizes.size()){
                    ofLog() <<getcluster();
                    r = rMult/5.0*getWidth()*sizes[getcluster()];
                }
                break;
            case 1:
                if(getEnv().size()>6){
                    r=rMult*getWidth()*getEnv(true)[6];
                }
                break;
        }
        if( Y <1){
            ofDrawEllipse(getWidth() * X,getHeight()/2,r,r);
        }
        else{
        ofDrawEllipse(getWidth() * X,getHeight()*(1+cos(2*3.1415*((int)(ofGetElapsedTimeMillis())%((int)(Y*10*1000.))) /(Y*10*1000.)))/4,r,r);
        }
        
    };
    
    
    
    
};

#endif
