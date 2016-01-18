//
//  SignalDrawer.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef __Hey__SignalDrawer__
#define __Hey__SignalDrawer__

#include "AppViz.h"
#include "ofApp.h"


class SignalDrawer:public AppViz{
    
public:
    SignalDrawer(ofApp* a):AppViz(a){
        params.setName("SignalDrawer");
        CPARAM(radius,30,1,100);
        CPARAM(onlyHPCP,true,false,true);
        
    };
    ~SignalDrawer(){};
    
    
    int stepX = 50;
    ofParameter<int> radius = 30;
    ofParameter<bool> onlyHPCP;
    float scaleY = 100;
    
    void draw() override{
        
        
        if(getHPCP().size()){
            stepX = getWidth()/((onlyHPCP?1:2)*getHPCP().size());
            scaleY = getHeight();
        }
        for(int i = 0 ; i < getHPCP().size() ; i++){
            ofDrawEllipse(i * stepX,getHeight()-getHPCP(true)[i]*scaleY,radius,radius);
            
        }
        
        
        if(!onlyHPCP){

        if(getMFCC().size()){
            stepX = getHeight()/(2*getMFCC().size());
            scaleY = getHeight();
        }
        for(int i = 0 ; i < getMFCC().size() ; i++){
            ofDrawRectangle(getWidth()/2 + i * stepX - radius/2,getHeight()-radius/2-getMFCC(true)[i]*scaleY,radius,radius);

        }
        }
        
        
    };
    
    
};
#endif /* defined(__Hey__SignalDrawer__) */
