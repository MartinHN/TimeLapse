//
//  Bars.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef Hey_Bars_h
#define Hey_Bars_h

#include "AppViz.h"
#include "ofApp.h"


class Bars:public AppViz{
public:
    Bars(ofApp* a):AppViz(a){
        params->setName("Bars");
        widthBar.set("widthBar",1,0,4);
        params->add(widthBar);

        
        typeHPCP.set("useHPCP",false,false,true);
        params->add(typeHPCP);
    };
    
    ofParameter<float> widthBar;
    ofParameter<bool> typeHPCP;
    
    void draw(){
        if(typeHPCP?hasHPCP():hasMFCC()){
            vector<float> arr = typeHPCP?getHPCP(true):getMFCC(true);
            float maxSpread = getWidth();
            float maxHeight = getHeight();
            float width = widthBar*maxSpread/(arr.size());

            int idx = 0;
            for(auto & a:arr){
                float cspread  = (idx+0.5)*maxSpread/(arr.size()+1);
                float cheight = a*maxHeight;
                ofDrawRectangle(cspread - width/2,maxHeight-cheight,width,cheight );
                idx ++;
            }
        }
    }
    
};

#endif
