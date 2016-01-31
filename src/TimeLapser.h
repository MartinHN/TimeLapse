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

// commandline execution
//#include <string>
//#include <iostream>
//#include <cstdio>
//#include <memory>

#define IM_W 1920
#define IM_H 1080


class TimeLapser : public AppViz{
public:
    
    TimeLapser(ofApp * a):AppViz(a){
        params.setName("TimeLapser");
        syphon.setup();
        syphon.set("LooperIn","Arena");
        CPARAM(period,50,50,1000);
        
        CPARAM(isRecording,false,false,true);
        CPARAM(isPlaying,false,false,true);
        CPARAM(saveRecorded,true,false,true);
        
        CPARAM(drawInput,false,false,true);
        CPARAM(isLooping,true,false,true);
        CPARAM(displayRecorded,true,false,true);
        CPARAM(clear,false,false,true);
        CPARAM(speed,1,0,50);
        CPARAM(bank,0,0,8);
        CPARAM(autoClear,true,false,true);
        isPlaying.addListener(this,&TimeLapser::beginPlay);
        isRecording.addListener(this,&TimeLapser::beginRec);
        clear.addListener(this,&TimeLapser::doClear);
        bank.addListener(this,&TimeLapser::setBank);
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
    
    static string getDataPathForBank(int b){
        static string path  = ofToDataPath("../../../TimeLapseBank/",true);
        string num = ofToString(b);
        return path+"/"+num+"/";
        
        
    }
    void draw()override {
        
        
        if(drawInput){
            
            syphon.draw(0,0);
        }
        else{
            if(_isPlaying && recorderThread.player.isLoaded()){
                recorderThread.player.update();
                recorderThread.player.draw(0,0,app->widthOut,app->heightOut);
            }
            else{
            image.draw(0,0,app->widthOut,app->heightOut);
            }
        }
    }
    ofxSyphonClient syphon;
    ofImage image;
    
    
    void doClear(bool & b){
        if(b){
            _isPlaying = false;
            
            recordHead=0;
            ofDirectory d(getDataPathForBank(bank));
            d.listDir();
            d.remove(true);
        }
    }
    
    void setBank(int & b){
        playHead = 0;
        recordHead = 0;
    }
    
    class Frame{
        public :
        Frame(){
            //      pix.allocate(IM_W,IM_H,OF_PIXELS_RGB);
            
            refTime =  0;
            duration = 200;
        }
//        ofPixels pix;
        float duration;
        unsigned long refTime;
    };
    
    void recordNew(ofTexture & t){
        syphon.bind();
        syphon.unbind();
        ofSetColor(255,255,255,255);
        fuck.begin();
        ofFill();
        ofClear(0);
        syphon.getTexture().draw(0,0);
        fuck.end();
        fuck.readToPixels(pixBuf);
        
        
        frames[recordHead].refTime = ofGetElapsedTimeMillis() - startRec;
        if(recordHead>0){
            frames[recordHead-1].duration = frames[recordHead].refTime - frames[recordHead-1].refTime;
            
        }
        if(displayRecorded || saveRecorded){
            image.setFromPixels(pixBuf);
            image.update();
            if(saveRecorded){
                string numS = ofToString(recordHead);
                if(recordHead <10){
                    numS = "0"+numS;
                    
                }
                if(recordHead < 100){
                    numS = "0"+numS;
                }
                string imgPath = getDataPathForBank(bank)+numS+".jpg";
                image.save(imgPath);
            }
        }
        
        recordHead = (recordHead+1)%frames.size();
    }
    
    
    void beginRec(bool & b){
        if(b){
            if(autoClear){
                bool dumb = true;
                doClear(dumb);
            }
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
            recorderThread.bank  = bank;
            if(recorderThread.isThreadRunning()){
                recorderThread.waitForThread();
            }
            recorderThread.frameRate = 1000.0/period;
            recorderThread.threadedFunction();
            
        }
    }
    
    

    

    void beginPlay(bool & p){
        _isPlaying = p;
        if(p)
            recorderThread.player.play();
        else
            recorderThread.player.stop();
        if(p){

            startPlay = localTime;
            playHead = 0;
            
        }
        else{
            
        }
    }
    void updatePlay(){
//        if(_isPlaying){
//            ofDirectory dir(getDataPathForBank(bank));
//            dir.listDir();
//            //                 if(playHead < recLength-1){
//            if(playHead<dir.getFiles().size()-1){
//                unsigned long nextTime = frames[playHead+1].refTime ;
//                if((localTime - startPlay) >=nextTime  ){
//                    playHead++;
//                    image.load(dir.getFiles()[playHead]);
//                    
//                }
//                
//            }
//            else if (isLooping){
//                playHead = 0;
//                startPlay = localTime;
//            }
//            
//            
//        }
    }
    
    
    ofParameter<bool> isRecording,isPlaying,drawInput,isLooping,displayRecorded,clear,saveRecorded,autoClear;
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
    
    class  ThreadedRecorder:public ofThread{
    public:
        int bank = 0;
        float frameRate;
        void threadedFunction() override{

            string command ;
            command = "cd "+TimeLapser::getDataPathForBank(bank)+";/usr/local/bin/ffmpeg -y -framerate "+ofToString(frameRate)+" -i %03d.jpg  -c:v mjpeg out.avi;";
            ofLog() << exec(command.c_str(),false);
            string loadPath=TimeLapser::getDataPathForBank(bank)+"out.avi";
            ofLog()<<loadPath;
            player.load(loadPath);
        }
        string exec(const char* cmd,bool returnRes = true) {
            std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
            if (!pipe) return "ERROR";
            char buffer[128];
            std::string result = "";
            while (returnRes && !feof(pipe.get())) {
                if (fgets(buffer, 128, pipe.get()) != NULL)
                    result += buffer;
            }
            return result;
        }
            ofVideoPlayer player;
    };

    ThreadedRecorder recorderThread;
    unsigned long lastRecTime;
    ofParameter<float> period;
    
};
#endif
