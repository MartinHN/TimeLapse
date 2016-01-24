//
//  TimeLapser.h
//  TimeLapse
//
//  Created by Atila on 21/01/16.
//
//

#ifndef TimeLapse_TimeLapser_h
#define TimeLapse_TimeLapser_h

#include "AppViz.h"
#include "ofApp.h"


#define IM_W 1920
#define IM_H 1080


class TimeLapser : public AppViz{
public:
    
    TimeLapser(ofApp * a):AppViz(a){
        params.setName("TimeLapser");
        syphon.setup();
        syphon.set("TimeLapseIn","Arena");
        CPARAM(period,500,500,2000);
        
        CPARAM(isRecording,false,false,true);
        CPARAM(isPlaying,false,false,true);
        CPARAM(saveRecorded,true,false,true);
        
        CPARAM(drawInput,false,false,true);
        CPARAM(isLooping,true,false,true);
        CPARAM(displayRecorded,true,false,true);
        CPARAM(clear,false,false,true);
        CPARAM(speed,1,0,50);
        CPARAM(bank,0,0,8);
        isPlaying.addListener(this,&TimeLapser::beginPlay);
        isRecording.addListener(this,&TimeLapser::beginRec);
        clear.addListener(this,&TimeLapser::doClear);
        frames.resize(1000);
        recordHead=0;
        playHead = 0;
        
        
        
    }
    void update()override{
        deltaTime = (ofGetElapsedTimeMillis() - lastTime)*speed;
        lastTime = ofGetElapsedTimeMillis();
        localTime+=deltaTime;
        checkSizes();
        if(needToCapture() ){
            //            syphon.bind();
            recordNew( syphon.getTexture());
            //            syphon.unbind();
        }
        
        updatePlay();
    }
    void checkSizes(){
        syphon.bind();
        syphon.unbind();
        if(syphon.getWidth()>0){
            
            
            if(!fuck.isAllocated() || (fuck.getWidth() != syphon.getWidth())){
                fuck.allocate(syphon.getWidth(),syphon.getHeight(),GL_RGB);
            }
            if(!image.isAllocated()||( image.getWidth() != syphon.getWidth())){
                pixBuf.allocate(syphon.getWidth(),syphon.getHeight(),OF_PIXELS_RGB);
                image.allocate(syphon.getWidth(),syphon.getHeight(),OF_IMAGE_COLOR);
            }
        }
    }
    
    bool needToCapture(){
        if(_isRecording && syphon.isSetup() && (ofGetElapsedTimeMillis() - lastRecTime>period)){
            lastRecTime = ofGetElapsedTimeMillis();
            return true;
            
        }
        return false;
    }
    
    string getDataPathForBank(int b){
        static string path  = ofToDataPath("../../../TimeLapseBank/",true);
        return path+"/"+ofToString(b)+"/";
        
        
    }
    void draw()override {
        
        
        if(drawInput){
            
            syphon.draw(0,0);
        }
        else{
            image.draw(0,0,app->widthOut,app->heightOut);
        }
    }
    ofxSyphonClient syphon;
    ofImage image;
    
    
    void doClear(bool & b){
        if(b){
            recordHead=0;
            ofDirectory d(getDataPathForBank(bank));
            d.listDir();
            d.remove(false);
        }
    }
    
    
    
    class Frame{
        public :
        Frame(){
            //      pix.allocate(IM_W,IM_H,OF_PIXELS_RGB);
            
            refTime =  0;
        }
        ofPixels pix;
        float duration;
        unsigned long refTime;
    };
    
    void recordNew(ofTexture * t){
        syphon.bind();
        syphon.unbind();
        ofSetColor(255,255,255,255);
        fuck.begin();
        ofFill();
        ofClear(0);
        syphon.getTexture()->draw(0,0);
        fuck.end();
        frames[recordHead].pix.allocate(fuck.getWidth(),fuck.getHeight(),OF_PIXELS_RGB);
        //        fuck.readToPixels(frames[recordHead].pix);
        fuck.readToPixels(pixBuf);
        frames[recordHead].refTime = ofGetElapsedTimeMillis() - startRec;
        if(recordHead>0){
            frames[recordHead-1].duration = frames[recordHead].refTime - frames[recordHead-1].refTime;
        }
        if(displayRecorded || saveRecorded){
            //            image.setFromPixels(frames[recordHead].pix);
            image.setFromPixels(pixBuf);
            image.update();
            if(saveRecorded){
                string imgPath = getDataPathForBank(bank)+ofToString(recordHead)+".jpg";
                image.save(imgPath);
            }
        }
        
        recordHead = (recordHead+1)%frames.size();
    }
    
    
    void beginRec(bool & b){
        if(b){
            _isRecording = true;
            startRec = ofGetElapsedTimeMillis();
            //            recordHead = 0;
        }
        
        else{
            _isRecording = false;
            if(recordHead>0){
                frames[recordHead-1].duration = frames[recordHead].refTime - frames[recordHead-1].refTime;
                frames[recordHead].duration = frames[recordHead-1].duration;
            }
            
            recLength = recordHead;
        }
    }
    
    void beginPlay(bool & p){
        _isPlaying = p;
        if(p){
            startPlay = localTime;
            playHead = 0;
            
        }
        else{
            
        }
    }
    void updatePlay(){
        if(_isPlaying){
            ofDirectory dir(getDataPathForBank(bank));
            dir.listDir();
            //                 if(playHead < recLength-1){
            if(playHead<dir.getFiles().size()){
                unsigned long nextTime = frames[playHead+1].refTime ;
                if((localTime - startPlay) >=nextTime  ){
                    playHead++;
                    //                         string imgPath = +ofToString(playHead)+".jpg";
                    //                         image.setFromPixels(frames[playHead].pix);
                    image.load(dir.getFiles()[playHead]);
                    //                         image.update();
                }
                
            }
            else if (isLooping){
                playHead = 0;
                startPlay = localTime;
            }
            
            
        }
    }
    
    
    ofParameter<bool> isRecording,isPlaying,drawInput,isLooping,displayRecorded,clear,saveRecorded;
    ofParameter<float> speed;
    ofParameter<int>bank;
    

    bool _isRecording = false,_isPlaying = false;
    unsigned long startRec;
    unsigned long startPlay;
    unsigned int recLength;
    unsigned int recordHead ;
    unsigned int playHead;
    unsigned int lastTime;
    unsigned long localTime;
    int deltaTime;
    
    ofPixels pixBuf;
    vector<Frame> frames;
    ofFbo fuck;
    
    
    
    unsigned long lastRecTime;
    ofParameter<float> period;
    
};
#endif
