//
//  TreeLapser.h
//  TimeLapse
//
//  Created by Atila on 25/01/16.
//
//

#ifndef TimeLapse_TreeLapser_h
#define TimeLapse_TreeLapser_h


#include "AppViz.h"
#include "ofApp.h"


class TreeLapser : public AppViz{
public:
    
    typedef ofVec2f ofVec;
    TreeLapser(ofApp * a):AppViz(a){
        params->setName("TreeLapser");
        CPARAM(startNew,false,false,true);
        CPARAM(clear,false,false,true);
        CPARAM(speed,1,0,5);
        CPARAM(fast,true,false,true);
        CPARAM(startPoint,ofVec(0.5,0),ofVec(0),ofVec(1));
        CPARAM(startOrient,90,0,360);
        CPARAM(width,10,10,300);
        startNew.addListener(this,&TreeLapser::doStartNew);
        clear.addListener(this,&TreeLapser::doClear);
        lastUpdateTime = ofGetElapsedTimeMillis();
    }
    
    
    ~TreeLapser(){
        bool dumb = true;
        doClear(dumb);
    }
    ofParameter<bool> startNew,clear,fast;
    ofParameter<float> speed,startOrient,width;
    
    ofParameter <ofVec> startPoint;
    
    
    class Branch{
    public:
        Branch(TreeLapser * _owner,Branch * p,ofVec _base,float _orientation,float _speed,float _width):owner(_owner),parent(p),base(_base),width(_width),orientation(_orientation),speed(_speed){
            childs.resize(0);
            curPoint = base;
            lastStepPoint = base;
            curOrientation = orientation;
            
        }
        
        
        bool growForTime(float t){
            _orVec = ofVec(cos(ofDegToRad(curOrientation)),sin(ofDegToRad(curOrientation)));
            curPoint += _orVec*owner->speed*(owner->fast?10:1);
            if(width>0.8 && length<maxLength){
                
                //                curLine.clear();
                curLine = generatePoly(lastStepPoint,curPoint);
                
                
                
                if((curPoint-lastStepPoint).length()>resolution){
                    path.newSubPath();
                    ofPolyline p = curLine.getOutline()[0];
                    for(int  i = 0 ; i  < p.size() ; i++){
                        path.lineTo(p[i]);
                    }
                    path.close();
                    
                    width*=0.95;
                    
                    curOrientation =  orientation+  ofRandom(-1.0f,1.0f) * 30.0;
                    lastStepPoint = curPoint;
                    
                    //                    if(maxLength>0 && ((maxLength/(resolution))%(300/(maxChild*resolution))==0)){
                    if(length > 50 && ofRandom(0,1)>0.8){
                        float newW = width;
                        if(newW>0.8 && childs.size() <  maxChild){
                            childs.push_back(new Branch(owner,this,curPoint,curOrientation + ofRandom(-1.0f,1.0f) * 30.0,speed*ofRandom(0.9,1.1),newW));
                        }
                    }
                    length+=resolution;
                }
                
                
                
                
            }
            for(auto &c:childs){
                if(c!=nullptr)
                    c->growForTime(t);
            }
        }
        
        
        ofPath generatePoly(ofVec s, ofVec e){
            ofPath poly;
            _orVec = ofVec(cos(ofDegToRad(curOrientation)),sin(ofDegToRad(curOrientation)));
            _orVec.normalize();
            ofVec normal(_orVec.y,-_orVec.x);
            normal*=width/2;
            poly.lineTo(lastStepPoint + normal);
            poly.lineTo(curPoint + normal);
            poly.lineTo(curPoint - normal);
            poly.lineTo(lastStepPoint - normal);
            poly.close();
            return poly;
            
        }
        
        
        
        
        void draw(){
            ofFill();
            ofSetLineWidth(1);
            
            path.draw();
            curLine.draw();
            for(auto& c:childs){
                if(c!=nullptr)
                    c->draw();
            }
        }
        
        
        void clear(){
            for(auto& c:childs){
                if(c!=nullptr)
                    c->clear();
            }
            for(auto & c:childs){
                delete c;
            }
            childs.clear();
        }
        
        int resolution = 10;
        float width,speed;
        ofPath path;
        vector<ofVec> ctlPoints;
        TreeLapser * owner;
        ofPath curLine;
        Branch * parent;
        vector<Branch *> childs;
        ofVec base,tip,curPoint;
        float orientation,curOrientation;
        ofVec _orVec;
        ofVec lastStepPoint;
        int maxChild = 2;
        int maxLength = 300;
        int length = 0;
        
        
    };
    vector<Branch *> roots;
    
    
    void doStartNew(bool & s){
        if(s){
            roots.push_back( new Branch(this,NULL, ofVec(startPoint->x *app->widthOut,startPoint->y*app->heightOut),startOrient,speed,width));
        }
    }
    void doClear(bool & c){
        if(c) {
            for(auto root:roots){
                root->clear();
                delete root;
            }
            roots.clear();
        }
    }
    void draw() override{
        for(auto root:roots){
            root->draw();
        }
    }
    void mouseReleased(int x,int y,int button){
            roots.push_back( new Branch(this,NULL, ofVec(x*1.0/ofGetWidth()*app->widthOut,y*1.0/ofGetHeight()*app->heightOut),startOrient,speed,width));
    }
    
    void update() override{
        for(auto root:roots){
            root->growForTime(ofGetElapsedTimeMillis() - lastUpdateTime);
        }
        lastUpdateTime = ofGetElapsedTimeMillis();
    }
    unsigned long lastUpdateTime;
};

#endif
