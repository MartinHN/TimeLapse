//
//  GloveInteract.h
//  ImMedia
//
//  Created by Tinmar on 28/10/2014.
//
//




// Base class for handling glove events



#ifndef __ImMedia__GloveInteract__
#define __ImMedia__GloveInteract__

#include <stdio.h>

#include "GloveOSC.h"


class GloveInteract  {
  

public:
    
    GloveInteract();
    virtual ~GloveInteract();
    
    // can be accessed by subclasses when notifyied
    static GloveInstance * curGlove;
    

  // from Glove events
    virtual void touch(const void* glove,touchEventArgs & a){
        curGlove=(GloveInstance*)glove;
        this->touch(a.touchId,a.state);
    };
    virtual void relativeMoved(const void* glove,ofVec3f & pos){
        curGlove = (GloveInstance*)glove;
        this->relativeMoved(pos);
    };
    virtual void cursor2DMoved(const void* glove,ofVec2f & pos){
        curGlove = (GloveInstance*)glove;
        this->cursor2DMoved(pos);
    };
    virtual void update(ofEventArgs & e){
        
        update();
    };
    
    virtual void swiped(const void* glove,swipeEventArgs & a){
        curGlove = (GloveInstance*)glove;
        this->swipe(a.touchId,a.axe,a.dir);
        
    }
    

    
    // overriden in sub class
    virtual void touch( TouchButton num, TouchAction s){};
    virtual void relativeMoved( ofVec3f v){};
    virtual void cursor2DMoved( ofVec2f v ){};

    virtual void swipe(TouchButton t,int axe,bool dir){};
    
    virtual void update(){};
    
    
    void removeGloveListeners(int priority);
    void addListeners(int & priority);
    int gloveEventsPriority,lastGloveEventsPrio;
    
    

};



#endif /* defined(__ImMedia__Draggable__) */
