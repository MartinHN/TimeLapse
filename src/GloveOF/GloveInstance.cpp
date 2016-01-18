//
//  GloveInstance.cpp
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//

#include "GloveInstance.h"



vector<GloveInstance*> GloveInstance::gloves;


extern ofEvent<ofEventArgs> drawSyphonEvent;

ofEvent<ofVec3f > GloveInstance::orientationEvent;
ofEvent<ofVec3f > GloveInstance::relativeOrientationEvent;
ofEvent<touchEventArgs > GloveInstance::touchEvent;
ofEvent< vector < float> > GloveInstance::flexEvent;
ofEvent<ofVec2f > GloveInstance::cursor2DEvent;
ofEvent<swipeEventArgs > GloveInstance::swipeEvent;
ofEvent<string > GloveInstance::calibEvent;

// may move that in utils or find equivalent in of
bool compare_vec( const ofVec2f& a, const ofVec2f& b )
{
    return a.x<b.x && a.y<b.y;
}


GloveInstance::GloveInstance(string _gloveID):gloveID(_gloveID){
    
    touchs = vector<bool>(NUMTOUCH+1,false);
    flex = vector<float>(NUMFLEX,0);
    
//    cursorSize = 180;
//    cursorImg =  ofImage();
//    cursorImg.load(ofToDataPath("UI/cursor/cursor.png"));
//    cursorImg.resize(cursorSize/2,cursorSize/2);
//    cursorImg.setAnchorPercent(.5,.5);
//    cursorImg_Action.load(ofToDataPath("UI/cursor/touch_2.png"));
//    cursorImg_Action.resize(cursorSize,cursorSize);
//    cursorImg_Action.setAnchorPercent(.5,.5);
//    cursorImg_Click.load(ofToDataPath("UI/cursor/touch_1.png"));
//    cursorImg_Action.resize(cursorSize,cursorSize);
//    cursorImg_Click.setAnchorPercent(.5,.5);
    
    
    
    ofAddListener(ofEvents().update, this, &GloveInstance::update);
    
    isActive = true;
    isFreezed = false;
}


GloveInstance::~GloveInstance(){
    
    ofRemoveListener(ofEvents().update, this, &GloveInstance::update);

}


void GloveInstance::update(ofEventArgs & a){
    
    if(MEDIAN_BUFFER_SIZE>0){
        cursorMedianX.push_back(cursor2DRaw.x);
        cursorMedianY.push_back(cursor2DRaw.y);
        
        for(int i = 0 ; i < cursorMedianX.size();i++){
                    if(ofVec2f((cursorMedianX[i],cursorMedianY[i])- cursor2DRaw.x).length()>MEDIAN_BUFFER_SIZE){
                        cursorMedianX.erase(cursorMedianX.begin() + i);
                        cursorMedianY.erase(cursorMedianY.begin() + i);
                        i--;
                    }
            }
        
        int maxSize = 60*4;
        if( cursorMedianX.size()>maxSize){
            cursorMedianX.erase(cursorMedianX.begin(),cursorMedianX.begin()+cursorMedianX.size()-maxSize);
        }
        if( cursorMedianY.size()>maxSize){
            cursorMedianY.erase(cursorMedianY.begin(),cursorMedianY.begin()+cursorMedianY.size()-maxSize);
        }
        
        vector<float> tmpX = cursorMedianX;
        vector<float> tmpY = cursorMedianY;
        ofSort(tmpX);
        ofSort(tmpY);
        cursor2D.x = cursorMedianX[tmpX.size()/2];
        cursor2D.y = cursorMedianY[tmpY.size()/2];
        
    }

}


void GloveInstance::draw(ofEventArgs & a){
    ofPushStyle();
    ofSetColor(255,255,255,cursorAlpha);
    if(!actionCursor && isActive){
//        cursorImg.draw(cursor2D.x,cursor2D.y);
//        if(touchs[GLOVE_BUTTON_ACTION])cursorImg_Action.draw(cursor2D.x,cursor2D.y);
//        if(touchs[GLOVE_BUTTON_CLICK])cursorImg_Click.draw(cursor2D.x,cursor2D.y);
    }
    
    drawRelativeInfos();
    if(gloveID !="mouse"){
        ofSetColor(isActive?0:255, isActive?255:0, 0);
        ofNoFill();
        ofSetLineWidth(5);
        ofDrawCircle(50, 20, 20);
    }
    ofPopStyle();
}




void GloveInstance::setOrientation(ofVec3f _orientation){
    orientationVel = _orientation - orientation;
    orientation = _orientation;
    if(!isCalib)ofNotifyEvent(orientationEvent,orientation ,this);
}

void GloveInstance::setRelativeOrientation(ofVec3f _r){
    float alphaVel = 1;
    
    if(ofGetElapsedTimef()-lastTouchDown<LOCK_AXE_TIME ){
        isLocking = true;
        if(_r.length()>0){
            float max =abs(_r[0]);
            lastLockedAxe = 0;
//            for(int i = 1 ; i< 3 ; i++){
//                if(abs(_r[i]*(i==2?LOCK_ROLL_DAMP.get():1.0f))>=max){
//                    lastLockedAxe = i;
//                }
//            }
        }
    }
    else if(isLocking){
        lockedAxe = lastLockedAxe+1;
        
        cout << lockedAxe << " : " << LOCK_AXE_TIME << endl;
        isLocking = false;
    }
    
    
    relativeOrientationVel = (1-alphaVel) * relativeOrientationVel + alphaVel*(_r - relativeOrientation);
    relativeOrientation = _r;
    if(!isCalib)ofNotifyEvent(relativeOrientationEvent, relativeOrientation,this);
}

void GloveInstance::setTouch(TouchButton tid,TouchAction state){
    
    touchEventArgs a;
    a.touchId = (TouchButton)tid;
    a.state = state;
    
    
    
    // press
    if(state == GLOVE_ACTION_DOWN){
        touchs[tid] = true;
        lastTouchDown = ofGetElapsedTimef();
//        if(tid == GLOVE_BUTTON_ACTION){
//            actionCursor=true;
//            isFreezed =true;
//            if(GloveInteractBox::hovered[this] !=NULL){
//                vector<string> v = ofSplitString(GloveInteractBox::hovered[this]->name, "/");
//                if(v[0]=="GUI"){
//                    isFreezed = false;
//                }
//            }
//            
//        }
        
        
        lastActionCursor = cursor2D;
        
    }
    
    
    //    All Other actions sets touch to off
    else touchs[tid] = false;
    
    
    // release
    if(state == GLOVE_ACTION_UP){
        
        
        
//        if(tid == GLOVE_BUTTON_ACTION){
//            actionCursor=false;
//            isFreezed = false;
//        }
        
        
        
        if(true){//SWIPE_HACK==1 || (lockedAxe>0 &&abs(relativeOrientation[lockedAxe-1])>10 && ofGetElapsedTimef()-lastTouchDown< SWIPE_MAX_TIME)){
            swipeEventArgs se;
            se.touchId = (TouchButton)tid;
            se.axe = 1;
            se.dir = relativeOrientation[se.axe-1] <0;
            if(!isCalib)ofNotifyEvent(swipeEvent,se, this);
            
            
        }
        lockedAxe = 0;
        
        if(!isCalib)ofNotifyEvent(touchEvent,a,this);
        
        
        // prevent hanging in undefinedState but wait to have sent event before
        relativeOrientation.set(0);
    }
    else{
        if(!isCalib)ofNotifyEvent(touchEvent,a,this);
    }
    
    
    
    
    
    
}

void GloveInstance::setFlex(int n,float f){
    flex[n] = f;
    if(!isCalib)ofNotifyEvent(flexEvent, flex,this);
}

void GloveInstance::setCursor2D(ofVec2f c){
    // freeze cursor during clicks
    
//    cursorAlpha = 255;
//    Tweener.removeTween(cursorAlpha);
//    Tweener.addTween(cursorAlpha,0,3);
//    ofRectangle full = cursorLim;
//    cursor2DVel = c*ofVec2f(full.x,full.y)-cursor2D;
    int pad = 1;
    cursor2DRaw.set( ofClamp(c.x,0,1),
                 ofClamp(c.y,0,1));
//
//    if(MEDIAN_BUFFER_SIZE==0){
    cursor2D =cursor2DRaw;
//    }
//    
//    
//  
//    
    if(!isCalib&&!isFreezed)ofNotifyEvent(cursor2DEvent, c,this);
    
    
}

//
//void GloveInstance::setCursor2DFromGoss(int WallIdx,ofVec2f c){
//    // freeze cursor during clicks
//    
//    cursorAlpha = 255;
//    Tweener.removeTween(cursorAlpha);
//    Tweener.addTween(cursorAlpha,0,3);
//    ofRectangle full = *screensHandler.fullScreen;
//    cursor2DVel = c*ofVec2f(full.x,full.y)-cursor2D;
//    
//    cursor2DRaw.set( ofClamp(screensHandler.screens[2][WallIdx]->x + c.x*screensHandler.screens[2][WallIdx]->width , 1 , full.width-1),
//                 ofClamp(cursor2D.y = c.y*full.height,1,full.height-1));
//    
//    if(MEDIAN_BUFFER_SIZE==0){
//        cursor2D =cursor2DRaw;
//    }
//    
//    
//    if(!isCalib &&!isFreezed)ofNotifyEvent(cursor2DEvent, cursor2D,this);
//    
//    
//    
//}






GloveInstance* GloveInstance::getGlove(string gloveID){
    
    for(vector<GloveInstance*>::iterator it = gloves.begin();it!=gloves.end();++it){
        if((*it)->gloveID==gloveID)return *it;
        
    }
    return NULL;
}

void GloveInstance::deleteGlove(string gloveID){
    for(vector<GloveInstance*>::iterator it = gloves.begin();it!=gloves.end();++it){
        if((*it)->gloveID==gloveID){
            delete *it;
            gloves.erase(it);
            return;
        }
        
    }
    
}


void GloveInstance::deleteAllGloves(){
    
    for(vector<GloveInstance*>::iterator it = gloves.begin();it<gloves.end();++it){
        if((*it)->gloveID!="mouse"){
            bool isLast = (it==gloves.end());
            delete *it;
            gloves.erase(it);
            isLast != (it==gloves.end());
            if(isLast)break;
        }
    }
}

bool GloveInstance::gloveExists(string gloveID){
    for(vector<GloveInstance*>::iterator it = gloves.begin();it!=gloves.end();++it){
        if((*it)->gloveID==gloveID){
            return true;
            
            
        }
        
    }
    return false;
}




void GloveInstance::drawRelativeInfos(){
    //    if(relativeOrientation.length()>0 && (int)lockedAxe == 3){
    //        ofPushStyle();
    //
    //        ofSetColor(255);
    //        ofPolyline arc;
    //        int rayon = 4*cursorSize;
    //        ofSetLineWidth(5);
    //        int anglePlus=30;
    //        arc.arc(cursor2D, rayon,rayon,-180-anglePlus,anglePlus,40);
    //
    //
    //        float rotation =relativeOrientation.z-90;
    //
    //        if(touchs[GLOVE_BUTTON_CLICK] && GloveInteractBox::dragged[this]!=NULL){
    //                    arc.draw();
    //            GloveInteractBox* cur =GloveInteractBox::dragged[this];
    //            ofTranslate(cursor2D);
    //                ofRotate(-(180+anglePlus));
    //            for(int i = 0 ; i< 4 ; i++){
    ////                if(i==0){ }
    //                ofRotate((180+2*anglePlus)/5);
    //                cursorImg.draw(rayon,0);
    ////                ofDrawCircle(rayon,0,cursorSize/2);
    //                if(i==cur->nextZoomLevel)        cursorImg_ArcCircle_Discret.draw(ofVec2f(rayon,0));
    //            }
    //
    //                ofRotate(-(180+2*anglePlus)*4.0/5);
    //
    //            ofRotate((cur->zoomLevel+1)*(180+2*anglePlus)/5);
    //            cursorImg_Center_Discret.draw(0,0);
    //            ofRotate((cur->zoomLevel+1)*(180+2*anglePlus)/5);
    //            ofTranslate(-cursor2D);
    //
    //
    //
    //        }
    //        else if (touchs[GLOVE_BUTTON_ACTION]){
    //                    arc.draw();
    //            ofTranslate(cursor2D);
    //            ofRotate(rotation);
    //            cursorImg_ArcCircle_Continu.draw(ofVec2f(rayon,0));
    //            cursorImg_Center_Continu.draw(0,0);
    //            ofRotate(-rotation);
    //            ofTranslate(-cursor2D);
    //        }
    //
    //        ofPopStyle();
    //
    //    }
    //
}






