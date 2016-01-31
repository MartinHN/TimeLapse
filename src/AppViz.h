//
//  AppViz.h
//  Hey
//
//  Created by Atila on 28/10/15.
//
//

#ifndef __Hey__AppViz__
#define __Hey__AppViz__

#include "ofMain.h"
#include "ofxPanel.h"

#define CPARAM(x,v,m,M) x.set(#x,v,m,M);params.add(x);



class ofApp;


class AppViz {
    public :
    AppViz(ofApp * a):app(a){
        params.setName("viz_default");
//        CPARAM(smooth, 0.1,0,.5);
//        CPARAM(smoothDown,0.1,0,.5);
//        CPARAM(SubMean,true,false,true);
        initListeners();
        
        

    }
    virtual  ~AppViz(){
        removeListeners();
    };
    
    ofParameterGroup params;
    void internUpdate(){
        HPCP = getHPCP();
        MFCC = getMFCC();
        enveloppes = getEnv();
        smoothArrays();
    };
    void smoothArrays(){

        if(HPCP.size()!=HPCP_s.size()){HPCP_s = HPCP;}
        float mean =0;for(auto & f:HPCP){mean+=f;}
        mean/=HPCP.size();
        for(int i = 0 ; i < HPCP.size() ; i++){
            float val = SubMean?HPCP[i]-mean:HPCP[i];
            if(HPCP_s[i]<val){HPCP_s[i] = ofLerp(HPCP_s[i],val,smooth);}
            else{HPCP_s[i] = ofLerp(HPCP_s[i],val,smoothDown);}
        }
        if(MFCC.size()!=MFCC_s.size()){MFCC_s = MFCC;}
        
        mean =0;for(auto & f:MFCC){mean+=f;}
        mean/=MFCC.size();
        for(int i = 0 ; i < MFCC.size() ; i++){
            float val = SubMean?MFCC[i]-mean:MFCC[i];
            if(MFCC_s[i]<val){MFCC_s[i] = ofLerp(MFCC_s[i],val,smooth);}
            else{MFCC_s[i] = ofLerp(MFCC_s[i],val,smoothDown);}
        }
        
        
        if(enveloppes.size()!=enveloppes_s.size()){enveloppes_s = enveloppes;}
        
        mean =0;for(auto & f:enveloppes){mean+=f;}
        mean/=enveloppes.size();
        for(int i = 0 ; i < enveloppes.size() ; i++){
            float val = SubMean?enveloppes[i]-mean:enveloppes[i];
            if(enveloppes_s[i]<val){enveloppes_s[i] = ofLerp(enveloppes_s[i],val,smooth);}
            else{enveloppes_s[i] = ofLerp(enveloppes_s[i],val,smoothDown);}
        }
        
        
    }
    
    void initListeners(){
        ofAddListener(ofEvents().mouseDragged,this,&AppViz::mouseDragged);
        ofAddListener(ofEvents().mousePressed,this , &AppViz::mousePressed);
        ofAddListener(ofEvents().mouseReleased,this , &AppViz::mouseReleased);
        ofAddListener(ofEvents().keyReleased,this , &AppViz::keyReleased);
            ofAddListener(ofEvents().keyPressed,this , &AppViz::keyPressed);
    }
    void removeListeners(){
        ofRemoveListener(ofEvents().mouseDragged,this,&AppViz::mouseDragged);
        ofRemoveListener(ofEvents().mousePressed,this , &AppViz::mousePressed);
        ofRemoveListener(ofEvents().mouseReleased,this , &AppViz::mouseReleased);
        ofRemoveListener(ofEvents().keyReleased,this , &AppViz::keyReleased);
                ofRemoveListener(ofEvents().keyPressed,this , &AppViz::keyPressed);
    }
    virtual void setup(){};
    virtual void update(){};
    virtual void draw(){};
    
    
    vector<float> & getHPCP(bool smooth = false);
    vector<float> & getMFCC(bool smooth = false);
    vector<float> & getEnv(bool smooth = false);
    int  & getcluster();
    
    bool hasMFCC();
    bool hasHPCP();
    bool hasCluster();
    bool hasEnv();
    
    float getWidth();
    float getHeight();
    vector<float> HPCP;
    vector<float> MFCC;
    vector<float> enveloppes;
    int cluster;
    
    vector<float> HPCP_s;
    vector <float> MFCC_s;
    vector<float> cluster_s;
    vector<float> enveloppes_s;
    
    
    ofParameter<float> smooth,smoothDown;
    ofParameter<bool> SubMean;
    
    
    

    
    
    
    virtual void windowResized(ofResizeEventArgs & resize){
        windowResized(resize.width,resize.height);
    }
    
    virtual void keyPressed( ofKeyEventArgs & key ){
        keyPressed(key.key);
    }
    virtual void keyReleased( ofKeyEventArgs & key ){
        keyReleased(key.key);
    }
    int mouseX,mouseY;
    virtual void mouseMoved( ofMouseEventArgs & mouse ){
        mouseX=mouse.x;
        mouseY=mouse.y;
        mouseMoved(mouse.x,mouse.y);
    }
    virtual void mouseDragged( ofMouseEventArgs & mouse ){
        mouseX=mouse.x;
        mouseY=mouse.y;
        mouseDragged(mouse.x,mouse.y,mouse.button);
    }
    virtual void mousePressed( ofMouseEventArgs & mouse ){
        mouseX=mouse.x;
        mouseY=mouse.y;
        mousePressed(mouse.x,mouse.y,mouse.button);
    }
    virtual void mouseReleased(ofMouseEventArgs & mouse){
        mouseX=mouse.x;
        mouseY=mouse.y;
        mouseReleased(mouse.x,mouse.y,mouse.button);
    }
    virtual void mouseScrolled( ofMouseEventArgs & mouse ){
        mouseScrolled(mouse.x,mouse.y);
    }
    virtual void mouseEntered( ofMouseEventArgs & mouse ){
        mouseEntered(mouse.x,mouse.y);
    }
    virtual void mouseExited( ofMouseEventArgs & mouse ){
        mouseExited(mouse.x,mouse.y);
    }

    
    virtual void windowResized(int w, int h){}
    
    virtual void keyPressed( int key ){}
    virtual void keyReleased( int key ){}
    
    /// \brief Called on the active window when the mouse is moved
    virtual void mouseMoved( int x, int y ){}
    
    /// \brief Called on the active window when the mouse is dragged, i.e.
    /// moved with a button pressed
    virtual void mouseDragged( int x, int y, int button ){}
    
    /// \brief Called on the active window when a mouse button is pressed
    virtual void mousePressed( int x, int y, int button ){}
    
    /// \brief Called on the active window when a mouse button is released
    virtual void mouseReleased(int x, int y, int button ){}
    
    /// \brief Called on the active window when the mouse wheel is scrolled
    virtual void mouseScrolled( float x, float y ){}
    
    /// \brief Called on the active window when the mouse cursor enters the
    /// window area
    ///
    /// Note that the mouse coordinates are the last known x/y before the
    /// event occurred, i.e. from the previous frame
    virtual void mouseEntered( int x, int y ){}
    
    /// \brief Called on the active window when the mouse cursor leaves the
    /// window area
    ///
    /// Note that the mouse coordinates are the last known x/y before the
    /// event occurred, i.e. from the previous frame
    virtual void mouseExited( int x, int y){}
    
    ofApp * app;
};

#endif /* defined(__Hey__AppViz__) */
